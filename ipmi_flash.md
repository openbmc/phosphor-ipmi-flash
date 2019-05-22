# BMC-side state machine details

***ONLY ONE BLOB IS ALLOWED OPEN AT A TIME***

To avoid excessive complications, there is a restriction that only one blob
within the firmware handler can be open at a time.

The system starts in the `notYetStarted` state.

At each state, each method has a specific effect, depending on a variety of
details, and sometimes the behavior is consistent regardless of overall state.

## `notYetStarted`

**The starting state.**

## `uploadInProgress`

**The BMC is expecting to receive bytes.**

## `verificationPending`

**The BMC is ready for verification or more bytes.**

## `verificationStarted`

**The verification process has started, no more writes allowed.**

## `verificationCompleted`

**The verification process has completed.**

## `updatePending`

**The update process is pending.**

## `updateStarted`

**The update process has started.**

## `updatedCompleted`

**The update has completed (optional state to reach)**
