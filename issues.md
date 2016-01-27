#Current issues for discussion

- It is undecidable to show the CURRENT rule being parsed. The best I can do is show the **next** rule to be parsed.

** THIS COULD BE SOLVED BY GOING FORWARD TO THE REDUCE THAT MAKES THE STATE STACK ONE SHORTER THAN THE CURRENT STACK (MORE SPECIFICALLY, THE STATE BEFORE THE CURRENT SHOULD BE THE LAST ONE ON THE STACK).** This is not easy but can be done.


- As a result, the `rulepos` command will now show all possible options of where one might be.



