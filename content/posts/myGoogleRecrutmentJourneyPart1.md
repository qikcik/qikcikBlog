# My Google Recruitment Journey, Part 1: Compressing Algorithmic Debt into a 7-Day Speedrun

## Introduction 
**About 2 months ago, an email from xwf.google.com dropped into my inbox**,
referencing an application from a year prior that I even forgot about. 
My initial classification was that it is not possible and that this is just spam.
But after the screening call, the reality hit: I will have two online interviews (one technical, one behavioral) in just a week.
And not just a regular interview to another company, these will be interviews for a company
that I still consider as one of the top-of-the-world factory of engineers.

This was a critical state. I’ve worked as a software developer in telecommunications for a few years, focusing on high-level abstraction: 
routing, message processing, and **writing business logic**. 
In my hobbyist gamedev projects, even though sometimes I liked to make some pathfinding algorithm or to do a CPU 3D rasterizer by hand, 
**at the end of the day my metric for success was simple: if it runs at >60 FPS without drops, it ships**. 

**My data structures were pragmatic**: flat vectors, statically sized arrays, sometimes simple maps, and for hard problems "SQLite". 
And all of my attempt at writing fancy algorithms or data structure  was a cool learning journey, that unfortunately did not have a direct impact on my job skills, 
and there were always better ways to spend this time if I wanted to optimize for a telecommunications career as well as for a gamedev career.

**Classical algorithms and strict LeetCode-style data structures were always outside my area of interest.**
My earliest attempts at learning them had failed completely. Even in my earliest memories on all primary school programming contests, I always failed at them.

**I had one week, a day job, other regular obligations and a fundamental knowledge gap.**

## Day 0: Making the plan and procrastination dressed as learning.
After some initial thinking (**I mean procrastination on YouTube** and online forums for at least a few hours for few days), 
I realized that the most successful strategy would be to learn as many patterns as possible - to at least **be able to identify the core problems**. 
I was aware that I had a very small chance of resolving it in the most optimal way during the interview, 
so I wanted to **at least understand what was expected from me to do**
(for my brain that is trained on writing software and resolving useful problems, mathematical description is not clear in a lot of places, 
and for me, it hides the clue, especially behind artificially created, non-real problems).


- I knew that if I start **reading an algorithm book**, I would fail because I didn't have time to deal with concepts obscured by pure math.
- I knew that if I start **reading online articles or watching YouTube explanation videos**, it would just be a form of procrastination.
- I knew that if I **just start doing LeetCode problems** (like in my previous attempts), I would burn a lot of time and energy without much outcome.

**The main problem at this point**: I was unable to solve even the simplest LeetCode problem. 
But this time, I knew the goal was not just to solve the problem (like during a game jam), but to learn as many patterns 
and concepts as possible even blindly without fully understanding all quirks for now.

## Day 1: Initial setup and warm-up
I started actual learning on friday. Based on previous thought **I decided to give a Large Language Model a chance to be my algorithmic teacher, since I never had a human one before**. 
I fed Gemini Pro chat with my exact situation (with my CV and preparation material provided by google in mail that i received) and set the following parameters for our session:
- **Protocol:** Iteratively explore and teach me new algorithmic concepts. Act like human private teacher that are fully commited in teaching me new concepts that I am not aware of.
- **Constraint:** Do not output any code
- **Objective:** Provide only conceptual hints and attack vectors for LeetCode problems, provide real-world examples to problems, and if it is easier to describe a problem using some metaphor - just do it.

These attempts resulted in successfully completing this set of problems suggested by LLM:
`Best Time to Buy and Sell Stock`, `Contains Duplicate`, `Valid Anagram`, `Group Anagrams`, `Product of Array Except Self`.

With hints, it occurred to me that these problems were simple usages of for-loops and arrays - but this gave me something that I never had before, courage that I am able to do even the simplest algorithm tasks. And I think that these are a very solid starting point because they bridged the gap between algorithms and problems that I had to do in "real" software, without being overwhelming. 
Moreover they showed me, not fully clearly for now but as a small hint, that sometimes a data structure like **"unordered_set" in these examples can be used in a not direct way to solve a problem in a clever way by using its not obvious properties**.
This also provided me some insights that a simple indexed lookup table like in `Valid Anagram` maybe from this perspective is equal in some property to an unordered_set in such a way that I never thought before.

Also, I think that equally important is the fact that **I started to show my full code solution to LLM, and ask for a better approach**, and the pros and cons of each possible implementation.

## Day 2: As much concept as possible
This was Saturday. **Assuming this might be my last deep-learning session, I changed the protocol to maximize throughput.**

I started a new chat where I instructed the LLM:
- First, the LLM will select the next concept; the goal is to learn as many common interview patterns as possible.
- After a strict 10-15 minute timebox, I will show my attempt. If I couldn't formulate an attack vector or crack the pattern, I will blindly accept the LLM's conceptual solution, asking for it in a second prompt while providing all my code attempts so far.
- The goal of the AI is to restructure my solution into an optimal and valid one.
- After I rewrote the LLM's solution, the LLM's role was to judge it and provide an alternative solution with code.
- Then there was space for discussion and clarification.
- And at the end, I rewrite the best possible solution to this problem in "my style".

**The hard rule for me was to never blindly rewrite AI code:** I had to write the code myself, using my own variable taxonomy and logic flow. I believe that forcing the solution into "my style" mapped the concepts deeper into my muscle memory, and (possibly I use this terms wrong) I believe that by doing this, I refactored my idiolect without forcing it to drop old behaviours.
Later, I learned that forcing "my flow" actually made me write code patterns that were not always valid, whereas keeping a strict order (like in BFS) is very useful and frees up a lot of thinking space for thinking about the problem (like checking "business" conditions before adding to the queue). But now, looking at this from a wider perspective, I believe that this was a necessary part of the process.

I gave myself a maximum of 30 minutes for each problem.
`Reverse Linked List, Linked List Cycle, Valid Palindrome, Invert Binary Tree, Longest Substring Without Repeating Characters, Valid Parentheses, Maximum Depth of Binary Tree, Merge Two Sorted Lists, Same Tree, Number of Islands, Find if Path Exists in Graph, Flood Fill, Reverse Integer, Single Number, Climbing Stairs`

**In doing these problems, I learned that:**
- Lists seem logical for more cases, and it is possible to just deduce a solution that is not the best, but not brute force, in most cases.
- I realized that tree problems are, under the hood, very similar to previous problems that I wrote earlier. Most of the traversal is a combination of BFS and DFS that I had done earlier in inter component logic and GUI DOM traversal.  For example, when I traversed by DFS, for me it was searching for a component that the mouse clicked on, and for BFS, it was maze solving. My initial solutions were not fully optimal, but I assumed that they were good enough (like storing the BFS element layer as a struct in the queue, instead of the math trick in which is done by iterating over queue.size() - I understood this pattern a lot, lot, lot of time later).
- Graphs, before that speedrun, seemed like one of the hardest for me, but something "clicked" as they appeared to me as a very common pattern that I deal with daily. Even if these problems do not go deeper into weird quirks like dealing with complex cycles, it seems not that hard, and for the hard parts, they started to look interesting and useful.
- There was only one Dynamic Programming problem `Climbing Stairs`, but it seems to be pure forbidden magic, powerful but dangerous, and honestly I completely did not understand it at this point.

Somewhere in the midpoint of this list, I noticed that the LLM's context started to degrade after around five problems. So **I separated one context for generating new problems, and a lot of smaller ones just to do 2-3 problems from the same domain (like graphs, or trees).**
Also, this was the hardest day, and to be honest, I rage-quit after Climbing Stairs. But during this day I spent something like 9 hours for learning, interrupted a lot by my other obligations.


## Day 3: Time to use learned knowledge

Time started running out, so I assumed this was a hard stop for learning entirely new concepts. Because the chance of getting an "Easy" LeetCode problem during interview was small, **I assumed I needed to feel comfortable with "Medium" problems.**

I generated a list of the most common interview tasks, assuming there is a high probability of some variation of them, and I also provided all my previously solved LeetCode problems to avoid duplication. 
And also, I decided that due to the fact that I will not have a chance to **click compile** during the interview, to offload checking common errors to the compiler, **I need to write the solution and send it to the LLM without compiling it and testing it in LeetCode.** 

The first three problems weren't so hard: `Merge k Sorted Lists`, `Merge Intervals`, `Design Add and Search Words Data Structure`. They required fewer hints than any easy ones. 

**I noticed that "Easy" problems are often the hardest because they introduce entirely new concept or pattern.** **The "Medium" problems I encountered were just trickier versions of the easy ones.**
The most important difference was that In most cases, I was able to write the code for medium problems without needing specific hints about the underlying concepts.

`Course Schedule` was a little bit tricky, but **after understanding the need for "inverting directions" and keeping track of "visiting" and "visited" states, writing it wasn't so hard** (it was hard, but not so much).  And also my eyes opened widely because right now it seems to me like a very common pattern that I could use in a lot of situations. 
Also, I think that a lot of work that I need to track manually can be compacted to some variation of this problem, like dependency tracking.

`Combination Sum` was my first interaction with backtracking during this speedrun. 
At this point, it wasn't so obvious to me, but I think that nevertheless, the correct order of previous problems like `Course Schedule` before that one, helped me to make a crack in my ignorance, because at least I could see that this weird backtracking stuff was useful in `Course Schedule` and maybe this is the same pattern but written differently - I think that was an important breakthrough for me.
**Seeing a standard way to handle some combinatorial explosion was eye-opening, instead of inventing the wheel for each problem like I used to do.**

`Container With Most Water` evoked strong emotion in me, because it seemed very similar to one from day one, but at this point, I was unable to write it myself... later it occurred to me that this is a similar but different, also powerful concept to attack the problem by going towards center from the corners (a variant of `two pointers`). 
**It opens my eyes to various ways of iteration different from just forward and backward, I connected it with the inversion of iteration in anagram problem.**

`Sum of Two Integers` was a very pleasant break from strong algorithmic problems. I wrote it as an iteration through binary operations that acts as a full binary adder.

I wanted to do at least one more dynamic programming problem LLM suggested `Unique Paths`. 
I assumed that I need to be aware of dynamic programming, but due to its incompatibility with my brain functioning at this point.
I made the decision that I need to put everything on other cards, and I had hopes that even if there will be a dynamic problem during the interview, it will be an easy, very generic one.

In an algorithmic sense, this subset of problems seems to not be that hard, **I was expecting exponential difficulty growth, or at least linear, and it seems that this was only multiplying difficulty by a constant.**
But my challenge of  not compiling code before sending it to the LLM was very depressing. **It occurred to me that I depend a lot on the compiler.**
In previous problems, the lack of an LSP was a problem, but this one was a way harder. At the end of the day, my biggest worry was that I would submit code during interview which do not work properly.
Also, **edge cases like off-by-one errors or mistaking ">=" with ">" in iteration started to be a real concern for me, but this was fully dependent on my fresh (not fully developed) muscle memory.**
I accepted that I am unable to resolve this in such a limited time, and I need to overcome it with narration and dialogue with the recruiter. 
It was weird that I started to make such mistakes, but it occurs to me that **I used to write loops in a very repetitive way, and these tasks forced me to think about iteration more fully with all possible ways instead of the few "robust" ones** that I used to do "in production".


## Days 4, 5, 6: Consolidating Acquired Knowledge Under a Hard Deadline

I didn't have much time, so I allocated it to just practicing patterns similar to the previous ones. This is the list the LLM generated for me:
`Two Sum, Longest Consecutive Sequence, Clone Graph, Palindromic Substrings.`
I also watched a few videos about the STL and common interview coding patterns. I prepared for the behavioral interview and read some STL documentation. I still don't know if this was the right decision at that point, or if it was just my way of procrastinating.

In the meantime, I tried to describe the "divide and conquer" concept to my friends, who had a school assignment that required this concept. 
After that, I decided to do one more binary search problem. The LLM chose `Find Minimum in Rotated Sorted Array` (I did it in two suboptimal binary searches, instead of a single one—which seemed to be pure magic to me at this point).

**The day before the interview, I decided not to look at any algorithms, and to just play games and watch casual YouTube videos.**
**I only looked at each of my previous solutions and tried to "actively" recreate the writing process**, but without focusing on one particular problem.

## The Interview

When the final day came, I was really shocked. **One week earlier, I thought LeetCode problems were just stupid, academic, math-like** problems where you needed to know weird tricks. 
But now, **I still somewhat thought that way... but I had completed around 34 LeetCode problems** (18 of them Medium and 1 Hard).

**I started noticing what they actually meant by "data structures"**. In this context, they are understood more as specific concepts with tricky properties used through an API to handle edge cases, rather than how they actually work under the hood.
Most of the problems were just tricky ways of describing a single issue **where the right approach is to find some weird constant relation between the data and to exploit it** as much as possible by using the right data structure and the right iteration method.

Dynamic programming was still black magic to me (it felt like using forgotten, forbidden, and dangerous ways to simplify problems) but I started to see them as way of some form of iteration with keeping the state.
I was also scared of all combinatorial problems and iterative ones, because, **there were a high chance that problem from this subset will require some math trick, that I do not know yet**.


I cannot share the exact assessment. Nevertheless, **the technical assessment hit exactly in the intersection of my preparation problem and interests: a hybrid of graph traversal and binary search**. 
It was the first time I faced this kind of problem, but I liked it from the start because it felt very much like a gamedev problem.
**During the discussion phase, I felt like I was applying an innovative approach to a real problem** (like I was making core mechanics for a new innovative RTS game).
In the second part, I proposed an iterative increasing approach, realizing mid-sentence **I could optimize it with binary search by slightly redefining the problem constraints without breaking them. The logic clicked.**

Then, the critical failure. **Under the stress of the running out of time, after successfully coding the graph traversal, my working memory dumped** the standard implementation for iterative binary search.
I knew the recursive version, but the iterative syntax vanished from my brain.

After some talk about that with the interviewer, **we agreed that the iterative approach is the only valid one at this stage.** 
The fact that I had only the last few minutes wasn't helpful in this situation.
**I fell back on the verbalization technique** I used with my friends. I reconstructed the binary search aloud, explaining to the interviewer exactly how I was slicing the problem space, even though the final code had mistakes (a lot of them).
I spotted them too late and I did not have time to refactor them.


## The Cooling, Waiting Time

**After the interview, I was in a state of superposition:** I simultaneously thought I had fully failed and somehow succeeded the interview. 
I realized that my speedrun was partially successful, but most of the learned knowledge wasn't available to me under stressful conditions.
This waiting phase for the interview results was very interesting: it allowed me to observe how I (and my colleagues) worked in my current telecommunications job, by keeping algorithmic concepts fresh in my mind due to the tension related to impatience for the results. I was also able to calmly structure a lot of the newly acquired knowledge and observations in my head.
For those few days, it was hard for me to write any code. For some reason, my brain had other plans and needed a cooldown.

**After a few days, I received a phone call.** I had already accepted that I had failed the interview and needed to return to my daily routine, which I had slightly neglected during that week.

**But the call brought unexpected news**: the recruiter mentioned I needed to pay more attention to code debuggability (whatever this means - I assume that under the corpo-language, they mean that I wrote invalid code), but nevertheless, they wanted to invite me to two on-site technical interviews.

**For me, at this point, it was both good and bad news.** I was stressed that this season in my life hadn't ended yet.
At the same time, I wanted to change my job. This desire stemmed mainly from the **realization that most of my colleagues were not aware of these algorithmic concepts in a way that could be applied to "real" software**. My perception of the whole industry was redefined after this speedrun. Before that, I thought that most programmers were aware of these concepts but purposely did not use them in "real" use cases - right now, I started to doubt that, and **I started to think that they treat this as two not overlapped sets of way of thinking**.

**But simultaneously, I was conflicted.** I like my current job, and I had duties that I would inevitably neglect to some degree during the next preparation phase. Furthermore, I had only been working at my current company for about 10 months.

## Epilogue

**The continuation will be in Part Two**, but there are some open questions, without clear answers, that live rent-free in my mind:

- **I am still amazed at how an LLM helped me understand a problem space I had been trying to grasp for over 10 years.** However, this is also sad, because it highlights how crucial a private tutor is for understanding concepts that aren't just a bucket of facts. **A human teacher would be better in this position.** Yet, it's fascinating how an LLM can describe a problem using words and concepts from entirely different domains. Why don't humans use this technique more often? I always loved teaching someone by using a metaphor that I knew would "click" in their brain, but that required at least a partial understanding of current life position and experiences of specific person.
- How is it that **we do not use this "forgotten and forbidden" coding in our daily production code, even though all highly reusable, useful code is essentially an exploitation of the intersection between classical algorithmic thinking and real-world problems?**
- Even if **a speedrun is the only way for me to break the initial wall** (without it, I was unable to see the algorithm world for over a decade), it is definitely not a valid way to build fluency and speed in solving problems. At the end of the day, is this fluency the only metric that actually matters? (Remember the part where I described the "divide and conquer" concept to my friends? **This remains a tricky observation for me:** thanks to that explanation, I was able to reconstruct binary search under stressful conditions, but at the same time, I had forgotten the simple code pattern itself.)

*(Also, a last interesting fact: writing this post took me around 8 hours and I wrote it months after the interview - I think that I should count it as a part of this speedrun.)*
