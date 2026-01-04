#ifndef SID_CONTROLLER_H

/**
 * Initializes the SID controller.
 * Call this method once to reset the controller.
 */
void sidControllerInitialize();

/**
 * Notifies the SID controller of an event.
 * @param sharedEvent The event identifier.
 */
void sidControllerProcessSharedEvent(int sharedEvent);

#ifdef TEST
void testSidController();
#endif

#endif	/* SID_CONTROLLER_H */

