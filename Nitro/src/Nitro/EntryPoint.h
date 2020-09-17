#pragma once

using namespace Nitro;

#ifdef NT_PLATFORM_WINDOWS
	#ifdef NT_WINDOWED_APP
		int CALLBACK WinMain(
			HINSTANCE hInstance,
			HINSTANCE prevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
		)
		{
			Base::Application* app = Base::CreateNitroApp();
			app->Run();
			delete app;
		}
	#else
		int main(int argc, char** argv)
		{
			Log::Init();
			NT_CORE_TRACE("log initialized.");
			NT_TRACE("log initilized.");
	
			Base::Application* app = Base::CreateNitroApp();
			app->Run();
			delete app;
		}
	#endif
#else
typedef int(*InitFunc)(int, char**);
typedef int(*ShutdownFunc)();

extern InitFunc NitroMain_Prologue;
extern ShutdownFunc NitroMain_Epilogue;

// @ Implemented by each project
int Main();

int NitroMain_RunOneIteration()
{
	int update_status = Main();
	return update_status;
}

int NitroMain(int argc, char** argv)
{
	if (NitroMain_Prologue(argc, argv))
	{
		int resume = 1;
		while (resume)
		{
			resume = NitroMain_RunOneIteration();
		}
		NitroMain_Epilogue();
	}
	return 0;
}
int main(int argc, char** argv)
{
	return NitroMain(argc, argv);
}
#endif