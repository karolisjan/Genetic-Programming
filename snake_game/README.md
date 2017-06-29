# AI for the Snake Game

<p align="center">
<table halign="center" width="543" border="1" cellspacing="1"><tbody><tr valign="top"><td width="23%">Objective:</td><td width="77%">Find a computer program that eats the maximum possible pieces of food.</td></tr><tr valign="top"><td>Terminal set:</td><td>Forward, Right, Left</td></tr><tr valign="top"><td>Function set:</td><td>IfFoodAhead,
		IfFoodUp,
		IfFoodRight,
		IfDangerAhead,
		IfDangerTwoAhead,
		IfDangerLeft,
		IfDangerRight,
		IfMovingUp,
		IfMovingDown,
		IfMovingLeft,
		IfMovingRight,
		Prog2,</td></tr><tr valign="top"><td>Fitness cases:</td><td>One fitness case.</td></tr><tr valign="top"><td>Raw Fitness:</td><td>Pieces of food eaten.</td></tr><tr valign="top"><td>Standardized fitness:</td><td>Maximum possible pieces of food eaten (217) minus the raw fitness.</td></tr><tr valign="top"><td>Hits:</td><td>Total pieces of food eaten during a run of the program, same as raw fitness.</td></tr><tr valign="top"><td>Wrapper:</td><td>None.</td></tr><tr valign="top"><td>Parameters:</td><td>popsize = 10000, generations = 1000, p_crossover = 0.9, p_mutation = 0.1, max_no_nodes = 100.</td></tr><tr valign="top"><td>Success predicate:</td><td>A program scores 217 hits.</td></tr></tbody>
</table> 
</p>

## References
[Application of Genetic Programming to the Snake Game](https://www.gamedev.net/articles/programming/artificial-intelligence/application-of-genetic-programming-to-the-snake-r1175)
