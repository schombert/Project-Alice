# How to report bugs (and get them fixed)

## The actual process

Leave a message in the `#testing` channel in the discord or, if you aren't on the discord, open an issue in the github repo.

## Making a useful bug report

Describing the issue that you encountered is in many ways the least useful part of a bug report. This is because the point of a bug report is not just to alert the developers that an issue exists, but to help them in fixing the issue. In some cases, a description of the problem itself is enough to figure out the root cause, but in most cases it is not. Thus, if you want to see the issue fixed, you probably need to provide us with some additional information.

The best thing you can do is to tell us the precise steps required to reproduce the issue. In other words, you need to give us a recipe that, when we follow it exactly, will let us see the issue for ourselves. If you can provide such a recipe, the issue you have encountered *will* be fixed, and probably in a quite timely manner. Note also that this recipe needs to start from scratch. If your bug requires a very specific game condition, you need to also tell us how to get the game into that condition. It simply isn't feasible to run hundreds of games trying to find the right conditions to see a bug.

Sometimes, however, that may not be possible. Keep in mind that, to fix an issue, a developer needs to be able to recreate it for themselves, so while you may not know the exact steps to reproduce it, the first part of any fix will be a developer figuring out those steps. To make a fix faster or more likely, try to provide as much potentially relevant information as possible. Try to describe what was happening leading up to the issue, especially anything that happened before hand that was out of the ordinary (by this, I don't mean "buggy", but rather anything that you did or encountered that isn't part of a typical game session).

## Bugs that will (probably) not be fixed

A bug report that is sufficiently vague (if you describe something as "weird" or "unusual" without further detail, for example): This is because the developers simply won't know what they are actually looking for, and it simply isn't worth anyone's time trying to find such a vaguely described problem.

A bug report that is basically a disagreement with either the rules governing the original game or intentional changes we have made to those rules: We are willing to discuss such issues, but they aren't bugs and shouldn't be reported as such.

Crash reports without steps to reproduce them: Such reports contain so little information that they simply aren't actionable. If you want to help us fix crashes, you need to build the project from the source code and run it under a debugger. Then, when you encounter a crash, you can report the stack trace to us as well as the state of the local variables. When you experience a crash under the debugger, try not to close the program or the debugger. Instead, keep it open so that we can ask you questions about the state of the program.