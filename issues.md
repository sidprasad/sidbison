#Current issues for discussion

- `crule` sometimes cannot identify a rule as it hits EOF. It makes a best effort to identify the rule. Maybe need need to take a look at how I check if the stack has changed, or even if that is a decent model. The solution  here is to take a 'step' and try again.

- As a result, the `rulepos` command will now show all possible options of where one might be. As well as the current rule. This is all the information the user requires, but could probably be better presented.

- Need to explore cutting down redundancy. Time and space complexity are obscenely high polynomials.

