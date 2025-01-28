#include "pch.h"

void* CreateFrameSinkAudio();
const wchar_t* GetWaveDevList();

namespace AudioRendInterop
{
	public ref class FrameSinkAudioNet
	{
	public:
		static System::IntPtr CreateInstanse()
		{
			void* ptr = CreateFrameSinkAudio();
			return System::IntPtr(ptr);
		}

		static System::String^ GetWaveDeviceList()
		{
			return gcnew System::String(GetWaveDevList());
		}

	}; // ref class FrameSinkAudioNet

} // namespace AudioRendInterop