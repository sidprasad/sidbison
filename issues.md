#Current issues for discussion

- It is undecidable to show the CURRENT rule being parsed. The best I can do is show the **next** rule to be parsed.

** THIS COULD BE SOLVED BY GOING FORWARD TO THE REDUCE THAT POPS THE CURRENT OF THE STATE STACK**

IE FIRST REDUCE AFTER WHICH THE STATE STACK LENGTH <= CURRENT LENGTH
WHERE IF LENGTHS ARE EQUAL, THE TOP STATES ON THE STACK ARE DIFFERENT


- As a result, the `rulepos` command will now show all possible options of where one might be.



