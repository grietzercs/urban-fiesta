/* chopsticks.h

   chopstick management routines interface,
   for Dining Philosophers example

*/

#define NTHREADS 5
/* the number of philosophers */

extern void chopsticks_init();
/* call this once, before calling any of the following subprograms 
 */
  
extern void chopsticks_take(int i);
/* Call this once before "eating".
   The effect is to grab the chopsticks at positions i and (i+1)mod 5.
   The caller will block until both chopsticks are available,
   then take the two chopsticks.  The two chopsticks are then not
   available for taking until they are released again.
 */

extern void chopsticks_put(int i);
/* Call this once after "eating".
   The effect is to release the chopsticks at positions i and (i+1)mod 5.
   This should only be called by a thread that has previously called
   chopsticks_take(i), and which has not called chopsticks_put since then.
 */

extern void chopsticks_finalize();
/* call this once, just before exiting
 */

extern void chopsticks_emergency_stop();
/* call this instead of chopsticks_finalize, in order
   to shut down everything
 */
  
