/* The threads of the FractumSeraph/OpenCE fork's platform layer (internet
 * play's, Delta's, the map hashes'), which it makes with pthread_create and
 * no attributes: platform.h sends those calls here on the 3DS, as the fork's
 * own 3DS port does (port/3ds/src/n3ds_posix_libc.c). A thread made without
 * attributes would get the C library's default stack, a few KB on the 3DS:
 * these get 256 KB (64 KB at least). They run on the application's core, a
 * priority below the thread that made them (the game's), so they run while
 * the game waits for the screen and never hold up its frame. */
#include <3ds.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

struct thread_start
{
    void *(*start)(void *);
    void *argument;
};

static void thread_trampoline(void *context)
{
    struct thread_start start = *(struct thread_start *)context;

    free(context);
    start.start(start.argument);
}

int halo_3ds_pthread_create(pthread_t *thread, const pthread_attr_t *attributes, void *(*start)(void *),
    void *argument)
{
    size_t stack_size = 256 * 1024;
    int detached = 0;
    s32 priority = 0x30;
    struct thread_start *context = malloc(sizeof(*context));
    Thread created;

    if (!context)
        return ENOMEM;
    if (attributes)
    {
        int state = PTHREAD_CREATE_JOINABLE;

        pthread_attr_getstacksize(attributes, &stack_size);
        if (pthread_attr_getdetachstate(attributes, &state) == 0)
            detached = state == PTHREAD_CREATE_DETACHED;
    }
    if (stack_size < 64 * 1024)
        stack_size = 64 * 1024;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    if (priority < 0x3F)
        priority++;
    context->start = start;
    context->argument = argument;
    /* (-2: the application's core) */
    created = threadCreate(thread_trampoline, context, stack_size, priority, -2, false);
    if (!created)
    {
        free(context);
        return EAGAIN;
    }
    if (detached)
        threadDetach(created);
    if (thread)
        *thread = (pthread_t)created;
    return 0;
}

int halo_3ds_pthread_detach(pthread_t thread)
{
    if (!thread || thread == (pthread_t)1)
        return EINVAL;
    threadDetach((Thread)thread);
    return 0;
}

int halo_3ds_pthread_join(pthread_t thread, void **result)
{
    if (result)
        *result = NULL;
    if (!thread || thread == (pthread_t)1)
        return EINVAL;
    threadJoin((Thread)thread, U64_MAX);
    threadFree((Thread)thread);
    return 0;
}

pthread_t halo_3ds_pthread_self(void)
{
    Thread current = threadGetCurrent();

    /* (the main thread, which libctru did not make, is 1) */
    return current ? (pthread_t)current : (pthread_t)1;
}

int halo_3ds_pthread_equal(pthread_t a, pthread_t b)
{
    return a == b;
}
