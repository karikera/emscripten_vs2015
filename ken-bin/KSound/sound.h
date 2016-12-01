#pragma once

#include <KR3/main.h>
#include <KRMessage/eventpump.h>

struct IDirectSoundBuffer;
typedef struct tWAVEFORMATEX WAVEFORMATEX;

namespace kr
{

	namespace sound
	{
		constexpr int secSample = 88200;
		constexpr int channel = 2;
		constexpr int sampleBit = 16;
		constexpr int blockAlignment = channel * sampleBit / 8;
		constexpr int secByte = blockAlignment * secSample;
		constexpr int maxVolume = (((dword)1) << (sampleBit - 1)) - 1;
		using sample = int_sz<sampleBit / 8>;

		inline dword secToByte(dword x) noexcept
		{
			return x * secByte;
		}

		class Sound;
		class PrimarySound;

		class Sound:public Keep<IDirectSoundBuffer>
		{
		public:
			using Keep::Keep;
			using Keep::operator =;

			bool createMonotone(dword dwSize, dword uVolume, dword uFrequency, int lPan) noexcept;
			dword loadFromWaveFile(pcwstr str) noexcept;
			dword loadFromOggFile(pcwstr str) noexcept;
			dword loadFromFileAuto(pcwstr str) noexcept;

			bool play() noexcept;
			bool dupPlay() noexcept;
			void safePlay() noexcept;
			
			// pump는 내부적으로 비동기 처리를 하기 위해 사용된다.
			static bool init(EventPump * pump) noexcept;
			static void deinit() noexcept;

		protected:
			void _create(const WAVEFORMATEX * format, dword size); // HResultException
			void _create(const WAVEFORMATEX * format, dword size, dword flags); // HResultException
		};

		class PrimarySound :public Sound
		{
		public:
			void create(); // HResultException
		};

	}
}
