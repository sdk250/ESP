#include "android_touch.hpp"

void Android_Touch(ImGuiIO& io)
{
	struct input_event touch_event;
	char* touchDevice = new char [NAME_SIZE];
	int fd = -1;
	float x, y, z;

	if (getTouchScreenDevice(touchDevice) != 0)
		perror("Error");

	fd = open(touchDevice, O_RDONLY);

	for (;;)
	{
		if (shutdown)
			break;
		if (read(fd, &touch_event, sizeof(touch_event)) == sizeof(touch_event))
		{
			switch(touch_event.code)
			{
				case ABS_MT_POSITION_X:
					x = touch_event.value;
					continue;
				case ABS_MT_POSITION_Y:
					y = touch_event.value;
					continue;
				case ABS_MT_TRACKING_ID:
					z = touch_event.value;
				default:
					break;
			}
			z < 0 ? io.MouseDown[0] = false : io.MouseDown[0] = true;
			switch (orientation)
			{
				case 0:
					io.MousePos = ImVec2(x, y);
					continue;
				case 1:
					io.MousePos = ImVec2(y , height - x);
					continue;
				case 3:
					io.MousePos = ImVec2(width - y, x);
				default:
					break;
			}
		}
	}

	delete [] touchDevice;
	close(fd);
	puts("\x1b[92;mTouch End.\x1b[0m");
}

int getTouchScreenDevice(char* fname)
{
	struct dirent** namelist;
	int i, ndev, ret = -1;
	if ((ndev = scandir("/dev/input", &namelist, isa_event_device, alphasort)) <= 0)
		return -1;
	for(i = 0; i < ndev; i++)
	{
		int fd = -1;
		unsigned long keybit[NBITS(KEY_CNT)];
		snprintf(fname, NAME_SIZE, "%s/%s", "/dev/input", namelist[i]->d_name);
		if((fd = open(fname, O_RDONLY)) < 0)
		{
			free(namelist[i]);
			continue;
		}
		memset(keybit, 0, sizeof(keybit));
		ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);
		close(fd);
		free(namelist[i]);
		if(CBIT(keybit, BTN_TOUCH) || CBIT(keybit, BTN_TOOL_FINGER))
			ret = 0;
	}
	return ret;
}

int isa_event_device(const struct dirent* dir)
{
	return strncmp("event", dir->d_name, 5) == 0;
}
