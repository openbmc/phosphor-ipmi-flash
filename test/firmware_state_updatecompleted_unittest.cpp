/* The goal of these tests is to verify the behavior of all blob commands given
 * the current state is UpdateCompleted.  This state is achieved as an exit from
 * updateStarted.
 *
 * This can be reached with success or failure from an update, and is reached
 * via a stat() call from updatedStarted.
 */
