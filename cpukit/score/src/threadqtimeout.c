/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*PAGE
 *
 *  _Thread_queue_Timeout
 *
 *  This routine processes a thread which timeouts while waiting on
 *  a thread queue. It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Thread_queue_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control       *the_thread;
  Thread_queue_Control *the_thread_queue;
  Objects_Locations     location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      the_thread_queue = the_thread->Wait.queue;

      /*
       *  If the_thread_queue is not synchronized, then it is either
       *  "nothing happened", "timeout", or "satisfied".   If the_thread
       *  is the executing thread, then it is in the process of blocking
       *  and it is the thread which is responsible for the synchronization
       *  process.
       *
       *  If it is not satisfied, then it is "nothing happened" and
       *  this is the "timeout" transition.  After a request is satisfied,
       *  a timeout is not allowed to occur.
       */

      if ( the_thread_queue->sync_state != THREAD_QUEUE_SYNCHRONIZED &&
           _Thread_Is_executing( the_thread ) ) {
        if ( the_thread_queue->sync_state != THREAD_QUEUE_SATISFIED )
          the_thread_queue->sync_state = THREAD_QUEUE_TIMEOUT;
      } else {
        the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
        _Thread_queue_Extract( the_thread->Wait.queue, the_thread );
      }
      _Thread_Unnest_dispatch();
      break;
  }
}
