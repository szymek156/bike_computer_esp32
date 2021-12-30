#include "abstract_task.h"

namespace bk {

// TODO: instead of queue_el_size, there could be a template it T, and then sizeof(T)
// Just a loose thought. Templates destroys layout of definition/declaration
// So probably I will never do it
AbstractTask::AbstractTask(size_t queue_el_size, size_t queue_size)
    : task_(nullptr),
      queue_(nullptr) {
    queue_ = xQueueCreate(queue_size, queue_el_size);
    configASSERT(queue_);
}

AbstractTask::~AbstractTask() {
    if (task_) {
        // Looks like TaskDelete is non-blocking, thread doesn't even have to leave the loop
        vTaskDelete(task_);
    }

    if (queue_) {
        vQueueDelete(queue_);
    }
}

QueueHandle_t AbstractTask::getQueue() {
    return queue_;
}

void AbstractTask::TaskLoop(void *arg) {
    AbstractTask *task = (AbstractTask *)arg;

    task->run();
}

void AbstractTask::start_execution(const char *name) {
    xTaskCreate(
        AbstractTask::TaskLoop, name, configIDLE_TASK_STACK_SIZE, this, tskIDLE_PRIORITY, &task_);
    configASSERT(task_);
}

TaskHandle_t AbstractTask::getTask() {
    return task_;
}
}  // namespace bk