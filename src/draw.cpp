#include "draw.hpp"

int main(int argc, char** argv)
{
	if (argc < 2)
		PID = get_pid(PACKAGE);
	else
		PID = get_pid(argv[1]);
	// printf("Pid: %d\n"
		// "Int: %zd bits\n"
		// "Long: %zd bits\n"
		// "Long long: %zd bits\n"
		// "Float: %zd bits\n"
		// "Double: %zd bits\n"
		// "void *: %zd bits\n"
		// "uintptr_t: %zd bits\n"
		// "uint64_t *: %zd bits\n"
		// "struct Vec3: %zd bits\n"
		// "struct Vec4: %zd bits\n",
		// PID, sizeof(int),
		// sizeof(long int), sizeof(long long int),
		// sizeof(float), sizeof(double),
		// sizeof(void *), sizeof(uintptr_t),
		// sizeof(uint64_t), sizeof(struct Vec3),
		// sizeof(struct Vec4)
	// );
	InitEGL* ie = nullptr;
	void* handle = nullptr;
	EGLDisplay EGL_display = EGL_NO_DISPLAY;
	EGLSurface EGL_surface = EGL_NO_SURFACE;
	ImGuiStyle style {};
	// bool show_window {false};
	ImVec4 clean_color = ImVec4(0, 0, 0, 0);
	ANativeWindow* addr = nullptr;
	ImFont* font = nullptr;
	char* FPS = nullptr;
	uint32_t origin_w = 0;
	uint32_t origin_h = 0;
	FILE* fp = nullptr;

	uint64_t game_addr = 0,
		matrix_p = 0,
		cLoc = 0,
		// fovPtr = 0,
		gname_buffer[30] {0},
		GName {0},
		enPtr = 0;
	float health_buffer[2];
	struct Vec3 xyz,
		exyz;
	struct uLevel ulevel;
	struct D3DMatrix vMat {};
	char* current_name = nullptr;
	char* Count = nullptr;
	char* distance_text = nullptr;

	current_name = new char [BUFFER_SIZE * 32];
	Count = new char [BUFFER_SIZE * 4];
	distance_text = new char [BUFFER_SIZE * 2];
	game_addr = get_module_base(LIBRARY);
	matrix_p = getA<uint64_t>(
		getA<uint64_t>(
			game_addr + 0xB550340
		) + 0xc0
	);
	cLoc = matrix_p + 0x750;
	// fovPtr = matrix_p + 0x660;
	matrix_p += 0x590;
	GName = getA<uint64_t>(
		getA<uint64_t>(
			game_addr + 0xB0EAE50
		) + 0x110
	);
	getB(&gname_buffer, GName, sizeof(gname_buffer));

	std::string window_size = exec("wm size");
	sscanf(window_size.c_str(), "Physical size: %ux%u", &width, &height);
	origin_w = width;
	origin_h = height;
	FPS = new char [BUFFER_SIZE * 8];
	std::thread* Orientation = new std::thread {[&] {
		for (;;orientation = atoi(
			exec("dumpsys input | awk '/SurfaceOrientation/{print $2}'").c_str()
		))
		{
			if (shutdown)
				break;
			switch (orientation)
			{
				case 0:
					width = origin_w;
					height = origin_h;
					break;
				case 1:
				case 3:
					width = origin_h;
					height = origin_w;
					break;
			}
			sleep(1);
		}
	}};
	Orientation->detach();

	if (!(fp = fopen("/data/local/tmp/libnwindow.so", "w")))
		perror("Library create");
	fwrite(libnwindow_so_7, sizeof(unsigned char), libnwindow_so_7_len, fp);
	fclose(fp);
	if (!(fp = fopen("/data/local/tmp/Heiti.ttf", "w")))
		perror("TTF create");
	fwrite(_ttf, sizeof(unsigned char), _ttf_len, fp);
	fclose(fp);

	handle = dlopen("/data/local/tmp/libnwindow.so", RTLD_LAZY);
	addr = ((ANativeWindow* (*)
		(const char*, uint32_t, uint32_t, android::PixelFormat, uint32_t))
		dlsym(handle, "createNativeWindow"))(
		"sdk250",
		width + height,
		height + width,
		android::PIXEL_FORMAT_RGBA_8888,
		(uint32_t) 0
	);
	printf("Surface address: %p\n", addr);

	ie = new InitEGL;
	EGL_display = ie->get_egl_display();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	font = io.Fonts->AddFontFromFileTTF(
		"/data/local/tmp/Heiti.ttf",
		40.0f,
		nullptr,
		io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
	);
	ImGui::StyleColorsDark();
	EGL_surface = ie->get_egl_surface(addr);
	if (!ImGui_ImplAndroid_Init(addr))
	{
		delete ie;
		perror("Error: cannot initialized ImGUI for the Android\nDetail");
		exit(EXIT_FAILURE);
	}
	ImGui_ImplOpenGL3_Init("#version 300 es");
	style = ImGui::GetStyle();
	style.ScaleAllSizes(10.0f);

	std::thread Touch {Android_Touch, std::ref(io)};
	Touch.detach();

	for (;;)
	{
		if (shutdown)
			break;

		// Draw
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplAndroid_NewFrame(width + height, height + width);
		ImGui::NewFrame();
		// if (show_window)
			// ImGui::ShowDemoWindow(&show_window);

		static uint32_t color_f = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 255.0f, 0.0f, 255.0f));
		static uint32_t _color_f = ImGui::ColorConvertFloat4ToU32(ImVec4(255.0f, 0.0f, 0.0f, 255.0f));
		static struct PlayerData Player_Count[MAX_PLAYER] {};
		static int changed = 0;
		static int myTeam = 101;
		static bool Rect {true};
		static bool Line {true};
		static bool Distance {true};
		int Person_Count = 0;
		int Bot_Count = 0;

		getB(&xyz, cLoc, sizeof(xyz));
		getB(&vMat, matrix_p, sizeof(vMat));
		// getB(data, (void *) fovPtr, sizeof(float));

		if (xyz.z == 88.441124f || xyz.z == 5278.43f || xyz.z == 88.440918f)
		{
			changed = 1;
			continue;
		}

		if (changed == 1)
		{
			enPtr = DecryptActorsArray(
				getA<uint64_t>(
					getA<uint64_t>(
						getA<uint64_t>(
							game_addr + 0xB5714E0
						) - 0x20
					) + 0x30
				),
				0xA0,
				0x448
			);
			changed = 0;
		}
		getB(&ulevel, enPtr, sizeof(ulevel));
		if (ulevel.size < 1 || ulevel.size > 0x1000 || isInvalid(ulevel.addr))
		{
			changed = 1;
			continue;
		}

		for (int i = 0; i < ulevel.size; i++)
		{
			uint64_t base_p = getA<uint64_t>(ulevel.addr + sizeof(uint64_t) * i);
			if (isInvalid(base_p))
				continue;

			if (getA<int>(base_p + sizeof(uint64_t)) != sizeof(uint64_t))
				continue;

			int ids = getA<int>(base_p + 3 * sizeof(uint64_t));
			int page = ids / 0x4000;
			int index = ids % 0x4000;

			if (page < 1 || page > 30)
				continue;

			if (gname_buffer[page] == 0)
				gname_buffer[page] = getA<uint64_t>(GName + page * sizeof(uint64_t));

			strcpy(current_name, getText(getA<uint64_t>(gname_buffer[page] + index * sizeof(uint64_t))));
			if (strlen(current_name) < 5)
				continue;

			if (strstr(current_name, "BP_PlayerPawn") || strstr(current_name, "BP_PlayerCharacter"))
			{
				int oType = getA<int>(base_p + 0x88); // Real person: 0x3ec2a00 Bot: 0x3ec2800
				if (oType != 0x3ec2a00 && oType != 0x3ec2800)
					continue;

				if (getA<bool>(base_p + Offsets::bDead))
					continue;

				Player_Count[Person_Count + Bot_Count].TeamID = getA<int>(base_p + Offsets::TeamID);

				int role = getA<int>(base_p + Offsets::Role);
				if (role == 258)
				{
					myTeam = Player_Count[Person_Count + Bot_Count].TeamID;
					continue;
				} else if (role != 257)
					continue;
				if (Player_Count[Person_Count + Bot_Count].TeamID == myTeam && myTeam <= 100)
					continue;

				uint64_t boneAddr = getA<uint64_t>(base_p + Offsets::Mesh);
				exyz = mat2Cord(getOMatrix(getA<uint64_t>(boneAddr + Offsets::MinLOD) + 0x60), getOMatrix(boneAddr + Offsets::FixAttachInfoList));
				Player_Count[Person_Count + Bot_Count].Pelvis = posCount(vMat, exyz);
				getB(
					&Player_Count[Person_Count + Bot_Count].isBot,
					base_p + Offsets::isBot,
					sizeof(Player_Count[Person_Count + Bot_Count].isBot)
				);
				getB(
					health_buffer,
					base_p + Offsets::Health,
					sizeof(health_buffer)
				);
				Player_Count[Person_Count + Bot_Count].Health = health_buffer[0] / health_buffer[1] * 100;
				Player_Count[Person_Count + Bot_Count].Distance = getDistance(xyz, exyz);

				if (health_buffer[1] > 200.0f ||
					health_buffer[1] < 50.0f ||
					health_buffer[0] > health_buffer[1] ||
					health_buffer[0] < 0.0f
				)
					continue;
				if (Player_Count[Person_Count + Bot_Count].Distance > 400.0f)
					continue;

				if (Player_Count[Person_Count + Bot_Count].Pelvis.w > 0.01f &&
					Player_Count[Person_Count + Bot_Count].Pelvis.x < width + 100 &&
					Player_Count[Person_Count + Bot_Count].Pelvis.x > -50
				)
				{
					// Player_Count[Person_Count + Bot_Count].Bone = getPlayerBone(base_p, vMat);
					// data->Weapon = getPlayerWeapon(pBase);
					strcpy(Player_Count[Person_Count + Bot_Count].PlayerName, "66:79:84:");

					if (!Player_Count[Person_Count + Bot_Count].isBot)
					{
						strcpy(
							Player_Count[Person_Count + Bot_Count].PlayerName,
							getNameByte(
								getA<uint64_t>(base_p + Offsets::PlayerName)
							).c_str()
						);
						if (strlen(Player_Count[Person_Count + Bot_Count].PlayerName) < 4)
							continue;
					}

					// x = r_x - (r_y - r_w) / 4;
					// y = r_y;
					// w = (r_y - r_w) / 2;
					// h = r_y - r_w;
					// TOP = y - w;
					// BUTTOM = y + w;
					int x = Player_Count[Person_Count + Bot_Count].Pelvis.x -
						(Player_Count[Person_Count + Bot_Count].Pelvis.y - Player_Count[Person_Count + Bot_Count].Pelvis.z) / 4;
					int y = Player_Count[Person_Count + Bot_Count].Pelvis.y;
					int w = (Player_Count[Person_Count + Bot_Count].Pelvis.y - Player_Count[Person_Count + Bot_Count].Pelvis.z) / 2;

					if (Line)
						ImGui::GetForegroundDrawList()->AddLine(
							ImVec2((float) width / 2, 100), // Begin
							ImVec2(
								x + w / 2,
								y - w
							), // End
							Player_Count[Person_Count + Bot_Count].isBot == true ? color_f : _color_f,
							2
						);
					if (Rect)
						ImGui::GetForegroundDrawList()->AddRect(
							ImVec2(
								x,
								y - w
							),
							ImVec2(
								x + w,
								y + w
							),
							Player_Count[Person_Count + Bot_Count].isBot == true ? color_f : _color_f,
							0,
							0,
							2.0f
						);
					if (Distance)
					{
						snprintf(distance_text, BUFFER_SIZE * 2, "%.1f m", Player_Count[Person_Count + Bot_Count].Distance);
						ImGui::GetForegroundDrawList()->AddText(
							font,
							29.0f,
							ImVec2((x + w / 2) - 30, y + w + 10),
							Player_Count[Person_Count + Bot_Count].isBot == true ? color_f : _color_f,
							distance_text
						);
					}
				}

				// Vec2 neck;
				// Vec2 cheast;
				// Vec2 pelvis;
				// Vec2 lSh;
				// Vec2 rSh;
				// Vec2 lElb;
				// Vec2 rElb;
				// Vec2 lWr;
				// Vec2 rWr;
				// Vec2 lTh;
				// Vec2 rTh;
				// Vec2 lKn;
				// Vec2 rKn;
				// Vec2 lAn;
				// Vec2 rAn;
				if (Player_Count[Person_Count + Bot_Count].Bone.on)
				{
					// ImGui::GetForegroundDrawList()->AddRect(
						// ImVec2(Player_Count[i].HeadLocaion.x
					// ImGui::GetForegroundDrawList()->AddLine(
						// ImVec2((float) width / 2, 100), // Begin
						// ImVec2(Player_Count[i].Bone.cheast.x, Player_Count[i].Bone.cheast.y), // End
						// Player_Count[i].isBot == true ? color_f : _color_f,
						// 1
					// );
					// ImGui::GetForegroundDrawList()->AddLine(
						// ImVec2((float) width / 2, 100), // Begin
						// ImVec2(Player_Count[i].Bone.neck.x, Player_Count[i].Bone.neck.y), // End
						// Player_Count[i].isBot == true ? color_f : _color_f,
						// 1
					// );
					ImGui::GetForegroundDrawList()->AddLine(
						ImVec2((float) width / 2, 100), // Begin
						ImVec2(
							Player_Count[Person_Count + Bot_Count].Bone.pelvis.x,
							Player_Count[Person_Count + Bot_Count].Bone.pelvis.y
						), // End
						Player_Count[Person_Count + Bot_Count].isBot == true ? color_f : _color_f,
						1
					);
					// ImGui::GetForegroundDrawList()->AddLine(
						// ImVec2((float) width / 2, 100), // Begin
						// ImVec2(Player_Count[i].Bone.lSh.x, Player_Count[i].Bone.lSh.y), // End
						// Player_Count[i].isBot == true ? color_f : _color_f,
						// 1
					// );
					// ImGui::GetForegroundDrawList()->AddLine(
						// ImVec2((float) width / 2, 100), // Begin
						// ImVec2(Player_Count[i].Bone.rSh.x, Player_Count[i].Bone.rSh.y), // End
						// Player_Count[i].isBot == true ? color_f : _color_f,
						// 1
					// );
				}

				if (Person_Count + Bot_Count >= MAX_PLAYER)
					continue;

				if (Player_Count[Person_Count + Bot_Count].isBot)
					Bot_Count++;
				else
					Person_Count++;
			}
		}

		ImGui::Begin("<-- START <--");

		ImGui::Text("Test version 0.1");
		// ImGui::Checkbox("Demo Window", &show_window);

		ImGui::Button("关闭") ? shutdown = true : 0x0;

		// ImGui::Text("X: %.2f, Y: %.2f", io.MousePos.x, io.MousePos.y);
		// ImGui::Text("Orientation: %d", orientation);

		ImGui::Text("find addr: %p", (void *) game_addr);
		ImGui::Checkbox("射线", &Line);
		ImGui::SameLine();
		ImGui::Checkbox("矩形", &Rect);
		ImGui::SameLine();
		ImGui::Checkbox("距离", &Distance);
		snprintf(Count, BUFFER_SIZE * 4, "真人: %d | 人机: %d", Person_Count, Bot_Count);
		ImGui::GetForegroundDrawList()->AddText(
			font,
			40.0f,
			ImVec2(width / 2 - 128, height / 13),
			color_f,
			Count
		);

		snprintf(
			FPS,
			BUFFER_SIZE * 8,
			"%.1f FPS",
			io.Framerate
		);
		ImGui::GetForegroundDrawList()->AddText(
			font,
			50.0f,
			ImVec2(width / 10, height / 13),
			color_f,
			FPS
		);

		ImGui::End();

		ImGui::Render();
		glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
		glClearColor(
			clean_color.x * clean_color.w,
			clean_color.y * clean_color.w,
			clean_color.z * clean_color.w,
			clean_color.w
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		eglSwapBuffers(EGL_display, EGL_surface);
	}

	delete ie;
	delete [] current_name;
	delete [] Count;
	delete [] distance_text;
	delete [] FPS;
	delete Orientation;
	sleep(1);
	dlclose(handle);
	ImGui_ImplAndroid_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	io.Fonts->Clear();
	ImGui::DestroyContext();
	exec("rm -f /data/local/tmp/libnwindow.so");
	exec("rm -f /data/local/tmp/Heiti.ttf");

	puts("\x1b[92;mDraw End.\x1b[0m");
	return 0;
}

std::string exec(const std::string& command)
{
	char buffer[BUFFER_SIZE * 16];
	std::string result {};
	// Open pipe to file
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		return "popen failed!";
	}
	// read till end of process:
	while (!feof(pipe)) {
		// use buffer to read and add to result
		if (fgets(buffer, BUFFER_SIZE * 16, pipe) != nullptr)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

uint64_t get_module_base(const char *module_name)
{
	FILE *fp;
	uint64_t addr = -1;
	char line[1024] {};
	char filename[256] {};
	snprintf(filename, sizeof(filename), "/proc/%d/maps", PID);
	fp = fopen(filename, "r");
	if (fp)
	{
		while (fgets(line, sizeof(line), fp))
		{
			uint64_t tmpBase;
			if (sscanf(line, "%" PRIXPTR "-%*" PRIXPTR
				" %*s %*s %*s %*s %s", &tmpBase, filename) > 0
			)
			{
				if (!strcmp(basename(filename), module_name))
				{
					addr = tmpBase;
					break;
				}
			}
		}
		fclose(fp);
	}
	return addr;
}

int get_pid(const char* package)
{
	char command[69];
	int pid = -1;
	sprintf(command, "pidof %s", package);
	pid = atoi(exec(command).c_str());
	if (pid < 10)
	{
		DIR* dir = NULL;
		struct dirent* ptr = NULL;
		FILE* fp = NULL;
		char filepath[256];
		char filetext[128];
		dir = opendir("/proc");
		if (NULL != dir)
		{
			while ((ptr = readdir(dir)) != NULL)
			{
				if ((strcmp(ptr->d_name, ".") == 0) ||
					(strcmp(ptr->d_name, "..") == 0) ||
					ptr->d_type != DT_DIR
				)
					continue;
				sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);
				fp = fopen(filepath, "r");
				if (NULL != fp)
				{
					fgets(filetext, sizeof(filetext), fp);
					if (strcmp(filetext, package) == 0)
					{
						pid = atoi(ptr->d_name);
						fclose(fp);
						break;
					}
					fclose(fp);
				}
			}
			closedir(dir);
		}
	}
	return pid;
}

inline struct D3DMatrix ToMatrixWithScale(
	struct Vec4 rot,
	struct Vec3 translation,
	struct Vec3 scale
)
{
	struct D3DMatrix m;

	float x2 = rot.x * 2;
	float y2 = rot.y * 2;
	float z2 = rot.z * 2;

	float x3 = rot.x * x2;
	float y3 = rot.y * y2;
	float z3 = rot.z * z2;

	float wx2 = rot.w * x2;
	float yz2 = rot.y * z2;

	float xy2 = rot.x * y2;
	float wz2 = rot.w * z2;

	float xz2 = rot.x * z2;
	float wy2 = rot.w * y2;

	m._41 = translation.x;
	m._42 = translation.y;
	m._43 = translation.z;

	m._11 = (1.0f - (y3 + z3)) * scale.x;
	m._22 = (1.0f - (x3 + z3)) * scale.y;
	m._33 = (1.0f - (x3 + y3)) * scale.z;

	m._23 = (yz2 + wx2) * scale.y;
	m._32 = (yz2 - wx2) * scale.z;

	m._12 = (xy2 + wz2) * scale.x;
	m._21 = (xy2 - wz2) * scale.y;

	m._13 = (xz2 - wy2) * scale.x;
	m._31 = (xz2 + wy2) * scale.z;

	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;
	m._44 = 1.0f;

	return m;
}

template <typename T>
inline T getA(uint64_t origin)
{
	T buffer {0};
	struct iovec local, remote;
	local.iov_base = (void *) &buffer;
	local.iov_len = sizeof(T);
	remote.iov_base = (void *) origin;
	remote.iov_len = sizeof(T);
	syscall(SYS_process_vm_readv, PID, &local, 1, &remote, 1, 0);
	return buffer;
}

inline void getB(void* target, uint64_t origin, size_t len)
{
	static struct iovec local, remote;
	local.iov_base = target;
	local.iov_len = len;
	remote.iov_base = (void *) origin;
	remote.iov_len = len;
	syscall(SYS_process_vm_readv, PID, &local, 1, &remote, 1, 0);
}

inline struct Vec2 _World2Screen(
	struct D3DMatrix viewMatrix,
	struct Vec3 pos
)
{
	struct Vec2 screen;
	float screenW = (viewMatrix._14 * pos.x) +
		(viewMatrix._24 * pos.y) + (viewMatrix._34 * pos.z) + viewMatrix._44;
	float screenX = (viewMatrix._11 * pos.x) +
		(viewMatrix._21 * pos.y) + (viewMatrix._31 * pos.z) + viewMatrix._41;
	float screenY = (viewMatrix._12 * pos.x) +
		(viewMatrix._22 * pos.y) + (viewMatrix._32 * pos.z) + viewMatrix._42;

	screen.y = (height / 2) - (height / 2) * screenY / screenW;
	screen.x = (width / 2) + (width / 2) * screenX / screenW;

	return screen;
}

inline float getDistance(struct Vec3 mxyz,struct Vec3 exyz)
{
	return sqrt((mxyz.x - exyz.x) * (mxyz.x - exyz.x) +
		(mxyz.y - exyz.y) * (mxyz.y - exyz.y) +
		(mxyz.z - exyz.z) * (mxyz.z - exyz.z)) / 100;
}

std::string getNameByte(uint64_t address)
{
	std::string lj {};
	unsigned short int nameI[32];
	getB(nameI, address, sizeof(nameI));
	char s[10];
	int i;
	for (i = 0; i < sizeof(nameI); i++)
	{
		if (nameI[i] == 0)
			break;
		sprintf(s, "%d:", nameI[i]);
		lj += s;
	}
	lj[63] = '\0';

	return lj;
}

char* getText(uint64_t addr)
{
	static char txt[64];
	memset(txt, '\0', sizeof(txt));
	char buff[63];
	getB(&buff, addr + 4 + sizeof(uint64_t), sizeof(buff));
	int i;
	for (i = 0; i < sizeof(buff) && buff[i] != 0; i++)
	{
		txt[i] = buff[i];
		if (buff[i] == 67 && i > 10)
			break;
	}
	txt[i + 1] = '\0';
	return txt;
}

inline bool isInvalid(uint64_t addr)
{
	return (addr < 0x1000000000 ||
		addr > 0xefffffffff ||
		(addr % sizeof(uint64_t)) != 0
	) ? true : false;
}

uint64_t DecryptActorsArray(uint64_t uLevel, int Actors_Offset, int EncryptedActors_Offset)
{
	if (uLevel < 0x10000000)
		return 0;

	if (getA<uint64_t>(uLevel + Actors_Offset) > 0)
		return uLevel + Actors_Offset;

	if (getA<uint64_t>(uLevel + EncryptedActors_Offset) > 0)
		return uLevel + EncryptedActors_Offset;

	auto Encryption = getA<Actors>(uLevel + EncryptedActors_Offset + 0x10);

	if (Encryption.Enc_1 > 0)
	{
		auto Enc = getA<Chunk>(Encryption.Enc_1 + 0x80);
		return ((
			(getA<uint8_t>(Encryption.Enc_1 + Enc.val_1) | (getA<uint8_t>(Encryption.Enc_1 + Enc.val_2) << 8))
			| (getA<uint8_t>(Encryption.Enc_1 + Enc.val_3) << 0x10)) & 0xFFFFFF
			| ((uint64_t)getA<uint8_t>(Encryption.Enc_1 + Enc.val_4) << 0x18)
			| ((uint64_t)getA<uint8_t>(Encryption.Enc_1 + Enc.val_5) << 0x20)) & 0xFFFF00FFFFFFFFFF
			| ((uint64_t)getA<uint8_t>(Encryption.Enc_1 + Enc.val_6) << 0x28)
			| ((uint64_t)getA<uint8_t>(Encryption.Enc_1 + Enc.val_7) << 0x30)
			| ((uint64_t)getA<uint8_t>(Encryption.Enc_1 + Enc.val_8) << 0x38);
	} else if (Encryption.Enc_2 > 0)
	{
		auto Encrypted_Actors = getA<uint64_t>(Encryption.Enc_2);
		if (Encrypted_Actors > 0)
		{
			return (uint16_t)(Encrypted_Actors - 0x400) & 0xFF00
				| (uint8_t)(Encrypted_Actors - 0x04)
				| (Encrypted_Actors + 0xFC0000) & 0xFF0000
				| (Encrypted_Actors - 0x4000000) & 0xFF000000
				| (Encrypted_Actors + 0xFC00000000) & 0xFF00000000
				| (Encrypted_Actors + 0xFC0000000000) & 0xFF0000000000
				| (Encrypted_Actors + 0xFC000000000000) & 0xFF000000000000
				| (Encrypted_Actors - 0x400000000000000) & 0xFF00000000000000;
		}
	} else if (Encryption.Enc_3 > 0)
	{
		auto Encrypted_Actors = getA<uint64_t>(Encryption.Enc_3);
		if (Encrypted_Actors > 0)
			return (Encrypted_Actors >> 0x38) | (Encrypted_Actors << (64 - 0x38));
	} else if (Encryption.Enc_4 > 0)
	{
		auto Encrypted_Actors = getA<uint64_t>(Encryption.Enc_4);
		if (Encrypted_Actors > 0)
			return Encrypted_Actors ^ 0xCDCD00;
	}
	return 0;
}

inline PlayerBone getPlayerBone(uint64_t pBase, struct D3DMatrix viewMatrix)
{
	struct PlayerBone b;
	b.on = true;
	struct D3DMatrix oMatrix {};
	uint64_t boneAddr = getA<uint64_t>(pBase + Offsets::Mesh);
	struct D3DMatrix baseMatrix = getOMatrix(boneAddr + Offsets::FixAttachInfoList);
	boneAddr = getA<uint64_t>(boneAddr + Offsets::MinLOD) + 0x30;
	// neck 0
	oMatrix = getOMatrix(boneAddr + 4 * 48);
	b.neck = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// cheast 1
	oMatrix = getOMatrix(boneAddr + 4 * 48);
	b.cheast = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// pelvis 2
	oMatrix = getOMatrix(boneAddr + 1 * 48);
	b.pelvis = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lSh 3
	oMatrix = getOMatrix(boneAddr + 11 * 48);
	b.lSh = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rSh 4
	oMatrix = getOMatrix(boneAddr + 32 * 48);
	b.rSh = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lElb 5
	oMatrix = getOMatrix(boneAddr + 12 * 48);
	b.lElb = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rElb 6
	oMatrix = getOMatrix(boneAddr + 33 * 48);
	b.rElb = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lWr 7
	oMatrix = getOMatrix(boneAddr + 63 * 48);
	b.lWr = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rWr 8
	oMatrix = getOMatrix(boneAddr + 62 * 48);
	b.rWr = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lTh 9
	oMatrix = getOMatrix(boneAddr + 52 * 48);
	b.lTh = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rTh 10
	oMatrix = getOMatrix(boneAddr + 56 * 48);
	b.rTh = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lKn 11
	oMatrix = getOMatrix(boneAddr + 53 * 48);
	b.lKn = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rKn 12
	oMatrix = getOMatrix(boneAddr + 57 * 48);
	b.rKn = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// lAn 13 
	oMatrix = getOMatrix(boneAddr + 54 * 48);
	b.lAn = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
	// rAn 14
	oMatrix = getOMatrix(boneAddr + 58 * 48);
	b.rAn = _World2Screen(viewMatrix, mat2Cord(oMatrix, baseMatrix));
//----------Credit -> @Shoolin_Official
	return b;
}

inline struct Vec3 mat2Cord(struct D3DMatrix pM1,struct D3DMatrix pM2)
{
	return (struct Vec3) {
		pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41,
		pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42,
		pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43
	};
}

inline struct D3DMatrix getOMatrix(uint64_t boneAddr)
{
	float oMat[11];
	getB(oMat, boneAddr, sizeof(oMat));

	return ToMatrixWithScale(
		(struct Vec4) {oMat[0], oMat[1], oMat[2], oMat[3]},
		(struct Vec3) {oMat[4], oMat[5], oMat[6]},
		(struct Vec3) {oMat[8], oMat[9], oMat[10]}
	);
}

inline struct Vec4 posCount(struct D3DMatrix vMat, struct Vec3 exyz)
{
	float camear_r = vMat._14 * exyz.x + vMat._24 * exyz.y + vMat._34 * exyz.z + vMat._44;
	return (struct Vec4) {
		(width / 2) + (vMat._11 * exyz.x + vMat._21 * exyz.y + vMat._31 * exyz.z +
		vMat._41) / camear_r * (width / 2),
		(height / 2) - (vMat._12 * exyz.x + vMat._22 * exyz.y + vMat._32 * (exyz.z - 5) +
		vMat._42) / camear_r * (height / 2),
		(height / 2) - (vMat._12 * exyz.x + vMat._22 * exyz.y + vMat._32 * (exyz.z + 205) +
		vMat._42) / camear_r * (height / 2),
		camear_r
	};
}
