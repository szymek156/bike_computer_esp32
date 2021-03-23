#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

namespace bk {

/** @brief Wrapping common stuff for tasks in the system.
 * FreeRTOS task handle and queue, used then by consumers to
 * gather data
 */
class AbstractTask {
 public:
    static const int QUEUE_SIZE = 1;
    /** @brief Creates common stuff Tasks are using
     * @param queue_el_size size in bytes of the element held in the queue
     * @param queue_size number of elements in the queue. 1 is the default.
     */
    AbstractTask(size_t queue_el_size, size_t queue_size = QUEUE_SIZE);
    virtual ~AbstractTask();

    /** @brief Do bootstrap operations and call start_execution at the end to become fully
     * operational */
    virtual void start() = 0;

    QueueHandle_t getQueue();

 protected:
    static void TaskLoop(void* arg);

    /** @brief Creates the task, starts the main loop execution.
     * @param name of the task. XXI century and we still do not have
     * reflection capabilities in that shitty language.
     * Fuck you Stroustrup, fuck you.
     */
    void start_execution(const char* name);

    /** @brief Main task control loop code goes here.
     */
    virtual void run() = 0;

    TaskHandle_t task_;
    QueueHandle_t queue_;
};
}  // namespace bk