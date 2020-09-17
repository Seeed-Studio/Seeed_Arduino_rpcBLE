
#include <string>
#include <sstream>
#include <iomanip>
#include "BLEFreeRTOS.h"
/**
 * Sleep for the specified number of milliseconds.
 * @param[in] ms The period in milliseconds for which to sleep.
 */
void BLEFreeRTOS::sleep(uint32_t ms)
{
	vTaskDelay(ms / portTICK_PERIOD_MS);
} // sleep

/**
 * Start a new task.
 * @param[in] task The function pointer to the function to be run in the task.
 * @param[in] taskName A string identifier for the task.
 * @param[in] param An optional parameter to be passed to the started task.
 * @param[in] stackSize An optional paremeter supplying the size of the stack in which to run the task.
 */
void BLEFreeRTOS::startTask(void task(void *), std::string taskName, void *param, uint32_t stackSize)
{
	xTaskCreate(task, taskName.data(), stackSize, param, 5, NULL);
} // startTask

/**
 * Delete the task.
 * @param[in] pTask An optional handle to the task to be deleted.  If not supplied the calling task will be deleted.
 */
void BLEFreeRTOS::deleteTask(TaskHandle_t pTask)
{
	vTaskDelete(pTask);
} // deleteTask

/**
 * Get the time in milliseconds since the %BLEFreeRTOS scheduler started.
 * @return The time in milliseconds since the %BLEFreeRTOS scheduler started.
 */
uint32_t BLEFreeRTOS::getTimeSinceStart()
{
	return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
} // getTimeSinceStart

/**
 * @brief Wait for a semaphore to be released by trying to take it and
 * then releasing it again.
 * @param [in] owner A debug tag.
 * @return The value associated with the semaphore.
 */
uint32_t BLEFreeRTOS::Semaphore::wait(std::string owner)
{
	xSemaphoreTake(m_semaphore, portMAX_DELAY);
	xSemaphoreGive(m_semaphore);
	return m_value;
} // wait

/**
 * @brief Wait for a semaphore to be released in a given period of time by trying to take it and
 * then releasing it again. The value associated with the semaphore can be taken by value() call after return
 * @param [in] owner A debug tag.
 * @param [in] timeoutMs timeout to wait in ms.
 * @return True if we took the semaphore within timeframe.
 */
bool BLEFreeRTOS::Semaphore::timedWait(std::string owner, uint32_t timeoutMs)
{
	auto ret = pdTRUE;
	ret = xSemaphoreTake(m_semaphore, timeoutMs);
	xSemaphoreGive(m_semaphore);
	return ret;
} // wait

BLEFreeRTOS::Semaphore::Semaphore(std::string name)
{
	m_semaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(m_semaphore);
	m_name = name;
	m_owner = std::string("<N/A>");
	m_value = 0;
}

BLEFreeRTOS::Semaphore::~Semaphore()
{
	vSemaphoreDelete(m_semaphore);
}

/**
 * @brief Give a semaphore.
 * The Semaphore is given.
 */
void BLEFreeRTOS::Semaphore::give()
{
	m_owner = std::string("<N/A>");
	xSemaphoreGive(m_semaphore);
} // Semaphore::give

/**
 * @brief Give a semaphore.
 * The Semaphore is given with an associated value.
 * @param [in] value The value to associate with the semaphore.
 */
void BLEFreeRTOS::Semaphore::give(uint32_t value)
{
	m_value = value;
	give();
} // give

/**
 * @brief Give a semaphore from an ISR.
 */
void BLEFreeRTOS::Semaphore::giveFromISR()
{
	BaseType_t higherPriorityTaskWoken;
	xSemaphoreGiveFromISR(m_semaphore, &higherPriorityTaskWoken);
} // giveFromISR

/**
 * @brief Take a semaphore.
 * Take a semaphore and wait indefinitely.
 * @param [in] owner The new owner (for debugging)
 * @return True if we took the semaphore.
 */
bool BLEFreeRTOS::Semaphore::take(std::string owner)
{
	bool rc = false;
	rc = ::xSemaphoreTake(m_semaphore, portMAX_DELAY) == pdTRUE;
	m_owner = owner;
	return rc;
} // Semaphore::take

/**
 * @brief Take a semaphore.
 * Take a semaphore but return if we haven't obtained it in the given period of milliseconds.
 * @param [in] timeoutMs Timeout in milliseconds.
 * @param [in] owner The new owner (for debugging)
 * @return True if we took the semaphore.
 */
bool BLEFreeRTOS::Semaphore::take(uint32_t timeoutMs, std::string owner)
{
	bool rc = false;
	rc = ::xSemaphoreTake(m_semaphore, timeoutMs / portTICK_PERIOD_MS) == pdTRUE;
	m_owner = owner;
	return rc;
} // Semaphore::take

/**
 * @brief Create a string representation of the semaphore.
 * @return A string representation of the semaphore.
 */
std::string BLEFreeRTOS::Semaphore::toString()
{
	char hex[9];
	std::string res = "name: " + m_name + " (0x";
	snprintf(hex, sizeof(hex), "%08x", (uint32_t)m_semaphore);
	res += hex;
	res += "), owner: " + m_owner;
	return res;
} // toString

/**
 * @brief Set the name of the semaphore.
 * @param [in] name The name of the semaphore.
 */
void BLEFreeRTOS::Semaphore::setName(std::string name)
{
	m_name = name;
} // setName
