// 잔광 (Afterglow) - 1인칭 공포 탈출 게임

#include "core/Application.h"

int main()
{
	ag::Application app;
	if (!app.Initialize())
	{
		app.Shutdown();
		return 1;
	}

	app.Run();
	app.Shutdown();
	return 0;
}