#ifndef MAIN_FREERTOS_H_
#define MAIN_FREERTOS_H_
#include "Seeed_Arduino_FreeRTOS.h"
#include <stdint.h>
#include <string>

/**
 * @brief Interface to %BLEFreeRTOS functions.
 */
class BLEFreeRTOS {
public:
	static void sleep(uint32_t ms);
	static void startTask(void task(void*), std::string taskName, void* param = nullptr, uint32_t stackSize = 2048);
	static void deleteTask(TaskHandle_t pTask = nullptr);

	static uint32_t getTimeSinceStart();

	class Semaphore {
	public:
		Semaphore(std::string owner = "<Unknown>");
		~Semaphore();
		void        give();
		void        give(uint32_t value);
		void        giveFromISR();
		void        setName(std::string name);
		bool        take(std::string owner = "<Unknown>");
		bool        take(uint32_t timeoutMs, std::string owner = "<Unknown>");
		std::string toString();
		uint32_t	wait(std::string owner = "<Unknown>");
		bool		timedWait(std::string owner = "<Unknown>", uint32_t timeoutMs = portMAX_DELAY);
		uint32_t	value(){ return m_value; };

	private:
		SemaphoreHandle_t m_semaphore;
		std::string       m_name;
		std::string       m_owner;
		uint32_t          m_value;
		bool              m_usePthreads;

	};
};

#endif /* MAIN_FREERTOS_H_ */
