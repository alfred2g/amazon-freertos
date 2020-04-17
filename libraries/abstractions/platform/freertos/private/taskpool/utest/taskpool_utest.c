#include <stdbool.h>
#include <stdlib.h>

#include "unity.h"

#include "portableDefs.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "mock_queue.h"
#include "mock_task.h"
#include "mock_timers.h"

#include "taskpool_freertos.h"

static void taskPoolCreate()
{
    uint8_t x;

    xQueueGenericCreateStatic_ExpectAnyArgsAndReturn( NULL );

    for( x = 0; x < ( UBaseType_t ) configTASKPOOL_NUMBER_OF_WORKERS; x++ )
    {
        xTaskCreateStatic_ExpectAnyArgsAndReturn( NULL );
    }

    taskPoolCreateSystemTaskPool();
}

/* called before each testcase */
void setUp( void )
{
    critical_entry = 0;
/*    assert_count = 0; */
}

/* called after each testcase */
void tearDown( void )
{
    TEST_ASSERT_FALSE( critical_entry );
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
    taskPoolCreate();
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return( numFailures > 0 );
}

static void taskPoolCallback( taskPoolJob_t * jobStorage,
                              void * userContext )
{
    ( void ) jobStorage;
    ( void ) userContext;
}


/*
 * static xTimerCreateStatic_cb(NULL, int timems, false, job, timecallback,
 *      job->timerstorage)
 * {
 * }
 */

/* ========================= Test Cases Begin ==============================*/

void test_task_pool_create_system_taskpool_already_running( void )
{
    taskPoolCreateSystemTaskPool();
}

void test_taskpool_initialize_job( void )
{
    BaseType_t ret;
    taskPoolJob_t * job = malloc( sizeof( taskPoolJob_t ) );

    taskPoolInitializeJob( taskPoolCallback, NULL, job );
    TEST_ASSERT_EQUAL( taskPoolCallback, job->userCallback );
    TEST_ASSERT_EQUAL( NULL, job->userContext );
    TEST_ASSERT_EQUAL( 1U, job->status );
    TEST_ASSERT_EQUAL( 1, taskPoolCounts.initialized );

    ret = taskPoolIsJobInitialised( job );
    TEST_ASSERT_EQUAL_INT( pdTRUE, ret );
    free( job );
}


void test_taskpool_schedule_deferred_timer( void )
{
    taskPoolJob_t * job = malloc( sizeof( taskPoolJob_t ) );

    taskPoolInitializeJob( taskPoolCallback, NULL, job );

    uint32_t timeMs = 34;
    taskPoolError_t ret;
    TimerHandle_t tmh;
    tmh = malloc( sizeof( TimerHandle_t ) );

    xTimerCreateStatic_ExpectAnyArgsAndReturn( tmh );
    xTaskGetTickCount_ExpectAndReturn( 3 );
    xTimerGenericCommand_ExpectAnyArgsAndReturn( 2 );
    ret = taskPoolScheduleDeferred( job, timeMs );
    TEST_ASSERT_EQUAL_INT( TASKPOOL_SUCCESS, ret );
    free( tmh );
    free( job );
}

void test_taskpool_schedule_deferred_queue( void )
{
    taskPoolJob_t * job = malloc( sizeof( taskPoolJob_t ) );
    uint32_t timeMs = 0UL;
    taskPoolError_t ret;

    taskPoolInitializeJob( taskPoolCallback, NULL, job );
    xQueueGenericSend_ExpectAnyArgsAndReturn( pdPASS );
    ret = taskPoolScheduleDeferred( job, timeMs );
    TEST_ASSERT_EQUAL( 1, taskPoolCounts.direct_dispatch );
    TEST_ASSERT_EQUAL_INT( TASKPOOL_SUCCESS, ret );
    free( job );
}

void test_taskpool_schedule_deferred_queue_failed( void )
{
    taskPoolJob_t * job = malloc( sizeof( taskPoolJob_t ) );
    uint32_t timeMs = 0UL;
    taskPoolError_t ret;

    taskPoolInitializeJob( taskPoolCallback, NULL, job );
    xQueueGenericSend_ExpectAnyArgsAndReturn( pdFAIL );
    ret = taskPoolScheduleDeferred( job, timeMs );
    TEST_ASSERT_EQUAL( 1, taskPoolCounts.direct_dispatch_failed );
    TEST_ASSERT_EQUAL_INT( TASKPOOL_FAILED_OPERATION, ret );
    free( job );
}

void test_taskpool_schedule_deferred_not_initialized( void )
{
    taskPoolJob_t * job = malloc( sizeof( taskPoolJob_t ) );
    uint32_t timeMs = 34;
    taskPoolError_t ret;
    TimerHandle_t tmh;

    tmh = malloc( sizeof( TimerHandle_t ) );

    ret = taskPoolScheduleDeferred( job, timeMs );
    TEST_ASSERT_EQUAL_INT( TASKPOOL_ILLEGAL_OPERATION, ret );
    TEST_ASSERT_EQUAL( 1, taskPoolCounts.schedule_illegal );

    free( tmh );
    free( job );
}

void test_taskpool_strerror( void )
{
    const char * message;

    message = taskPoolStrError( TASKPOOL_SUCCESS );
    TEST_ASSERT_EQUAL_STRING( "SUCCESS", message );

    message = taskPoolStrError( TASKPOOL_ILLEGAL_OPERATION );
    TEST_ASSERT_EQUAL_STRING( "OPERATION NOT ALLOWED", message );

    message = taskPoolStrError( TASKPOOL_FAILED_OPERATION );
    TEST_ASSERT_EQUAL_STRING( "OPERATION FAILED", message );

    message = taskPoolStrError( TASKPOOL_GENERAL_FAILURE );
    TEST_ASSERT_EQUAL_STRING( "GENERAL FAILURE", message );

    message = taskPoolStrError( 509 );
    TEST_ASSERT_EQUAL_STRING( "INVALID STATUS", message );
}
