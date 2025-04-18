# Getting Started (Work In Progress)

The GraphBLAS is is sparse linear algebra library with three core
objects, Matrices, Vectors and Scalars.  OneSparse provides
Postgres types that let you do sparse algebraic and graph
operations on these objects.  OneSparse is a binding to the
powerful
[SuiteSparse:GraphBLAS](http://faculty.cse.tamu.edu/davis/GraphBLAS.html)
library, which implements a Just-In-Time Compiler to target CPUs
and CUDA GPUs without changing any of the algebraic code.  The same
code that can be written and run on laptops with small datasets,
can be run on powerful multi-core GPU systems with no changes.

First we need some data.  Run the command `\i demo/fixtures.sql`
from a Docker Demo container to include the data used in this
guide:
``` postgres-console
psql:demo/fixtures.sql:1: NOTICE:  extension "onesparse" already exists, skipping
psql:demo/fixtures.sql:2: NOTICE:  table "ostest" does not exist, skipping
psql:demo/fixtures.sql:21: NOTICE:  table "karate" does not exist, skipping
psql:demo/fixtures.sql:29: NOTICE:  table "mbeacxc" does not exist, skipping
psql:demo/fixtures.sql:41: NOTICE:  table "bcsstk01" does not exist, skipping
psql:demo/fixtures.sql:49: ERROR:  extra data after last expected column
CONTEXT:  COPY bcsstk01, line 1: "0 0  2.83226851852e+06"
psql:demo/fixtures.sql:51: NOTICE:  table "ash219" does not exist, skipping
psql:demo/fixtures.sql:62: NOTICE:  table "fs_183_1" does not exist, skipping
psql:demo/fixtures.sql:79: NOTICE:  table "test_graphs" does not exist, skipping
```
The examples below will use the
[Newman/Karate](https://sparse.tamu.edu/Newman/karate) graph the
demo test fixtures:
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="660pt" height="323pt"
 viewBox="0.00 0.00 659.57 323.43" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 319.43)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-319.43 655.57,-319.43 655.57,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="398.72" cy="-189.73" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="398.72" y="-187.83" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="340.95" cy="-245.99" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="340.95" y="-244.09" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge1" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M388.92,-199.27C378.36,-209.55 361.65,-225.84 350.99,-236.21"/>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="268.07" cy="-184.74" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="268.07" y="-182.84" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge2" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M382.96,-189.12C357.85,-188.16 309.03,-186.3 283.89,-185.34"/>
</g>
<!-- 4 -->
<g id="node4" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="368.97" cy="-209.79" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="368.97" y="-207.89" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 1&#45;&#45;4 -->
<g id="edge3" class="edge">
<title>1&#45;&#45;4</title>
<path fill="none" stroke="black" d="M386.96,-197.65C385.02,-198.97 382.99,-200.33 381.04,-201.65"/>
</g>
<!-- 5 -->
<g id="node5" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="486.88" cy="-103.87" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="486.88" y="-101.97" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 1&#45;&#45;5 -->
<g id="edge4" class="edge">
<title>1&#45;&#45;5</title>
<path fill="none" stroke="black" d="M408.49,-180.21C425.44,-163.71 460.09,-129.96 477.07,-113.43"/>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="549.31" cy="-162.92" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="549.31" y="-161.02" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 1&#45;&#45;6 -->
<g id="edge5" class="edge">
<title>1&#45;&#45;6</title>
<path fill="none" stroke="black" d="M413.99,-187.01C442.85,-181.87 505.11,-170.79 534,-165.65"/>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="539.86" cy="-123.66" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="539.86" y="-121.76" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge6" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M412.25,-183.39C439.16,-170.79 499.12,-142.73 526.16,-130.07"/>
</g>
<!-- 8 -->
<g id="node8" class="node">
<title>8</title>
<ellipse fill="none" stroke="black" cx="355.47" cy="-165.44" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="355.47" y="-163.54" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&#45;8 -->
<g id="edge7" class="edge">
<title>1&#45;&#45;8</title>
<path fill="none" stroke="black" d="M385.98,-182.57C380.39,-179.44 373.86,-175.77 368.27,-172.63"/>
</g>
<!-- 9 -->
<g id="node9" class="node">
<title>9</title>
<ellipse fill="none" stroke="black" cx="242.49" cy="-207.47" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="242.49" y="-205.57" font-family="Times,serif" font-size="8.00">9</text>
</g>
<!-- 1&#45;&#45;9 -->
<g id="edge8" class="edge">
<title>1&#45;&#45;9</title>
<path fill="none" stroke="black" d="M383.17,-191.49C353.22,-194.89 287.87,-202.32 257.98,-205.71"/>
</g>
<!-- 11 -->
<g id="node10" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="500.45" cy="-149.67" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="500.45" y="-147.77" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 1&#45;&#45;11 -->
<g id="edge9" class="edge">
<title>1&#45;&#45;11</title>
<path fill="none" stroke="black" d="M412.87,-184.16C431.54,-176.8 464.35,-163.89 484.07,-156.12"/>
</g>
<!-- 12 -->
<g id="node11" class="node">
<title>12</title>
<ellipse fill="none" stroke="black" cx="517.61" cy="-256.44" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="517.61" y="-254.54" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1&#45;&#45;12 -->
<g id="edge10" class="edge">
<title>1&#45;&#45;12</title>
<path fill="none" stroke="black" d="M411.45,-196.86C433.73,-209.37 479.68,-235.15 503.14,-248.32"/>
</g>
<!-- 13 -->
<g id="node12" class="node">
<title>13</title>
<ellipse fill="none" stroke="black" cx="460.75" cy="-221.81" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="460.75" y="-219.91" font-family="Times,serif" font-size="8.00">13</text>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge11" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M411.88,-196.53C421.84,-201.68 435.52,-208.76 445.88,-214.12"/>
</g>
<!-- 14 -->
<g id="node13" class="node">
<title>14</title>
<ellipse fill="none" stroke="black" cx="300.38" cy="-201.43" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="300.38" y="-199.53" font-family="Times,serif" font-size="8.00">14</text>
</g>
<!-- 1&#45;&#45;14 -->
<g id="edge12" class="edge">
<title>1&#45;&#45;14</title>
<path fill="none" stroke="black" d="M382.92,-191.61C365.55,-193.67 337.65,-196.99 319.17,-199.19"/>
</g>
<!-- 18 -->
<g id="node14" class="node">
<title>18</title>
<ellipse fill="none" stroke="black" cx="433.89" cy="-279.03" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="433.89" y="-277.13" font-family="Times,serif" font-size="8.00">18</text>
</g>
<!-- 1&#45;&#45;18 -->
<g id="edge13" class="edge">
<title>1&#45;&#45;18</title>
<path fill="none" stroke="black" d="M403.32,-201.4C410.06,-218.52 422.55,-250.24 429.29,-267.35"/>
</g>
<!-- 20 -->
<g id="node15" class="node">
<title>20</title>
<ellipse fill="none" stroke="black" cx="285.55" cy="-246.75" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="285.55" y="-244.85" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 1&#45;&#45;20 -->
<g id="edge14" class="edge">
<title>1&#45;&#45;20</title>
<path fill="none" stroke="black" d="M385.29,-196.49C364.1,-207.17 323.01,-227.87 300.76,-239.09"/>
</g>
<!-- 22 -->
<g id="node16" class="node">
<title>22</title>
<ellipse fill="none" stroke="black" cx="403.14" cy="-303.41" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="403.14" y="-301.51" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&#45;22 -->
<g id="edge15" class="edge">
<title>1&#45;&#45;22</title>
<path fill="none" stroke="black" d="M399.19,-201.89C400.05,-223.77 401.82,-269.51 402.67,-291.33"/>
</g>
<!-- 32 -->
<g id="node17" class="node">
<title>32</title>
<ellipse fill="none" stroke="black" cx="230.46" cy="-114.73" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="230.46" y="-112.83" font-family="Times,serif" font-size="8.00">32</text>
</g>
<!-- 1&#45;&#45;32 -->
<g id="edge16" class="edge">
<title>1&#45;&#45;32</title>
<path fill="none" stroke="black" d="M385.24,-183.72C354.51,-170.02 279.42,-136.55 246.44,-121.85"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge17" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M330.2,-236.95C316.42,-225.37 292.67,-205.41 278.87,-193.81"/>
</g>
<!-- 2&#45;&#45;4 -->
<g id="edge18" class="edge">
<title>2&#45;&#45;4</title>
<path fill="none" stroke="black" d="M349.21,-235.32C352.82,-230.65 357.06,-225.18 360.68,-220.5"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge19" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M343.16,-233.76C345.9,-218.53 350.56,-192.69 353.29,-177.53"/>
</g>
<!-- 2&#45;&#45;14 -->
<g id="edge20" class="edge">
<title>2&#45;&#45;14</title>
<path fill="none" stroke="black" d="M331.95,-236.1C325.41,-228.92 316.58,-219.23 309.92,-211.91"/>
</g>
<!-- 2&#45;&#45;18 -->
<g id="edge21" class="edge">
<title>2&#45;&#45;18</title>
<path fill="none" stroke="black" d="M355.47,-251.15C372.12,-257.07 399.36,-266.76 416.95,-273.01"/>
</g>
<!-- 2&#45;&#45;20 -->
<g id="edge22" class="edge">
<title>2&#45;&#45;20</title>
<path fill="none" stroke="black" d="M325.22,-246.21C318.83,-246.29 311.37,-246.4 304.67,-246.49"/>
</g>
<!-- 2&#45;&#45;22 -->
<g id="edge23" class="edge">
<title>2&#45;&#45;22</title>
<path fill="none" stroke="black" d="M350.94,-255.22C362.21,-265.62 380.46,-282.47 392.13,-293.24"/>
</g>
<!-- 31 -->
<g id="node18" class="node">
<title>31</title>
<ellipse fill="none" stroke="black" cx="217.97" cy="-249.98" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="217.97" y="-248.08" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 2&#45;&#45;31 -->
<g id="edge24" class="edge">
<title>2&#45;&#45;31</title>
<path fill="none" stroke="black" d="M325.38,-246.5C302.88,-247.23 261.35,-248.58 237.05,-249.37"/>
</g>
<!-- 3&#45;&#45;4 -->
<g id="edge25" class="edge">
<title>3&#45;&#45;4</title>
<path fill="none" stroke="black" d="M282.96,-188.43C302.04,-193.17 334.92,-201.33 354.02,-206.08"/>
</g>
<!-- 3&#45;&#45;8 -->
<g id="edge26" class="edge">
<title>3&#45;&#45;8</title>
<path fill="none" stroke="black" d="M283.29,-181.38C299.33,-177.83 324.44,-172.29 340.42,-168.77"/>
</g>
<!-- 3&#45;&#45;9 -->
<g id="edge27" class="edge">
<title>3&#45;&#45;9</title>
<path fill="none" stroke="black" d="M257.67,-193.98C256.12,-195.36 254.52,-196.78 252.97,-198.16"/>
</g>
<!-- 3&#45;&#45;14 -->
<g id="edge29" class="edge">
<title>3&#45;&#45;14</title>
<path fill="none" stroke="black" d="M281.21,-191.53C282.61,-192.25 284.04,-192.99 285.46,-193.72"/>
</g>
<!-- 10 -->
<g id="node19" class="node">
<title>10</title>
<ellipse fill="none" stroke="black" cx="189.48" cy="-274.64" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="189.48" y="-272.74" font-family="Times,serif" font-size="8.00">10</text>
</g>
<!-- 3&#45;&#45;10 -->
<g id="edge28" class="edge">
<title>3&#45;&#45;10</title>
<path fill="none" stroke="black" d="M259.36,-194.7C244.41,-211.8 214.01,-246.57 198.71,-264.07"/>
</g>
<!-- 28 -->
<g id="node20" class="node">
<title>28</title>
<ellipse fill="none" stroke="black" cx="181.58" cy="-95.61" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="181.58" y="-93.71" font-family="Times,serif" font-size="8.00">28</text>
</g>
<!-- 3&#45;&#45;28 -->
<g id="edge30" class="edge">
<title>3&#45;&#45;28</title>
<path fill="none" stroke="black" d="M258.48,-174.86C242.03,-157.91 208.58,-123.43 191.74,-106.08"/>
</g>
<!-- 29 -->
<g id="node21" class="node">
<title>29</title>
<ellipse fill="none" stroke="black" cx="198.35" cy="-143.6" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="198.35" y="-141.7" font-family="Times,serif" font-size="8.00">29</text>
</g>
<!-- 3&#45;&#45;29 -->
<g id="edge31" class="edge">
<title>3&#45;&#45;29</title>
<path fill="none" stroke="black" d="M255.61,-177.38C243.54,-170.26 225.28,-159.49 212.63,-152.03"/>
</g>
<!-- 33 -->
<g id="node22" class="node">
<title>33</title>
<ellipse fill="none" stroke="black" cx="132.64" cy="-201.32" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="132.64" y="-199.42" font-family="Times,serif" font-size="8.00">33</text>
</g>
<!-- 3&#45;&#45;33 -->
<g id="edge32" class="edge">
<title>3&#45;&#45;33</title>
<path fill="none" stroke="black" d="M252.53,-186.64C227.52,-189.7 178.41,-195.72 151.45,-199.02"/>
</g>
<!-- 4&#45;&#45;8 -->
<g id="edge33" class="edge">
<title>4&#45;&#45;8</title>
<path fill="none" stroke="black" d="M365.28,-197.67C363.35,-191.31 360.98,-183.55 359.06,-177.24"/>
</g>
<!-- 4&#45;&#45;13 -->
<g id="edge34" class="edge">
<title>4&#45;&#45;13</title>
<path fill="none" stroke="black" d="M384.54,-211.83C400.4,-213.9 424.99,-217.13 441.93,-219.35"/>
</g>
<!-- 4&#45;&#45;14 -->
<g id="edge35" class="edge">
<title>4&#45;&#45;14</title>
<path fill="none" stroke="black" d="M353.41,-207.89C343.29,-206.66 330.03,-205.04 319.31,-203.74"/>
</g>
<!-- 5&#45;&#45;7 -->
<g id="edge36" class="edge">
<title>5&#45;&#45;7</title>
<path fill="none" stroke="black" d="M501.08,-109.17C508.68,-112.01 518.02,-115.5 525.63,-118.34"/>
</g>
<!-- 5&#45;&#45;11 -->
<g id="edge37" class="edge">
<title>5&#45;&#45;11</title>
<path fill="none" stroke="black" d="M490.45,-115.9C492.43,-122.58 494.88,-130.87 496.86,-137.56"/>
</g>
<!-- 6&#45;&#45;7 -->
<g id="edge38" class="edge">
<title>6&#45;&#45;7</title>
<path fill="none" stroke="black" d="M546.42,-150.93C545.25,-146.06 543.9,-140.44 542.73,-135.58"/>
</g>
<!-- 6&#45;&#45;11 -->
<g id="edge39" class="edge">
<title>6&#45;&#45;11</title>
<path fill="none" stroke="black" d="M534.39,-158.88C529.22,-157.47 523.39,-155.89 518.03,-154.44"/>
</g>
<!-- 17 -->
<g id="node23" class="node">
<title>17</title>
<ellipse fill="none" stroke="black" cx="632.47" cy="-121.34" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="632.47" y="-119.44" font-family="Times,serif" font-size="8.00">17</text>
</g>
<!-- 6&#45;&#45;17 -->
<g id="edge40" class="edge">
<title>6&#45;&#45;17</title>
<path fill="none" stroke="black" d="M562.67,-156.24C577.64,-148.76 601.85,-136.65 617.46,-128.85"/>
</g>
<!-- 7&#45;&#45;17 -->
<g id="edge41" class="edge">
<title>7&#45;&#45;17</title>
<path fill="none" stroke="black" d="M555.57,-123.26C571.47,-122.87 596.07,-122.25 613.15,-121.82"/>
</g>
<!-- 9&#45;&#45;31 -->
<g id="edge42" class="edge">
<title>9&#45;&#45;31</title>
<path fill="none" stroke="black" d="M236.05,-218.64C232.53,-224.74 228.18,-232.29 224.63,-238.45"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge43" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M226.75,-206.59C206.93,-205.48 172.92,-203.58 151.67,-202.39"/>
</g>
<!-- 34 -->
<g id="node24" class="node">
<title>34</title>
<ellipse fill="none" stroke="black" cx="149.8" cy="-195.11" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="149.8" y="-193.21" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 9&#45;&#45;34 -->
<g id="edge44" class="edge">
<title>9&#45;&#45;34</title>
<path fill="none" stroke="black" d="M226.77,-205.38C210.65,-203.23 185.6,-199.88 168.48,-197.6"/>
</g>
<!-- 14&#45;&#45;34 -->
<g id="edge46" class="edge">
<title>14&#45;&#45;34</title>
<path fill="none" stroke="black" d="M281.31,-200.63C252.5,-199.42 198.19,-197.14 169.2,-195.92"/>
</g>
<!-- 20&#45;&#45;34 -->
<g id="edge53" class="edge">
<title>20&#45;&#45;34</title>
<path fill="none" stroke="black" d="M269.18,-240.52C243.09,-230.6 192.36,-211.3 166.24,-201.36"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge76" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M219.24,-124.67C200.37,-141.37 162.51,-174.88 143.73,-191.51"/>
</g>
<!-- 32&#45;&#45;34 -->
<g id="edge77" class="edge">
<title>32&#45;&#45;34</title>
<path fill="none" stroke="black" d="M220.24,-124.91C204.82,-140.29 175.72,-169.28 160.19,-184.76"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge74" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M203.49,-241.73C187.71,-232.73 162.6,-218.41 146.9,-209.46"/>
</g>
<!-- 31&#45;&#45;34 -->
<g id="edge75" class="edge">
<title>31&#45;&#45;34</title>
<path fill="none" stroke="black" d="M206.1,-240.43C193.74,-230.48 174.48,-214.97 162,-204.93"/>
</g>
<!-- 10&#45;&#45;34 -->
<g id="edge45" class="edge">
<title>10&#45;&#45;34</title>
<path fill="none" stroke="black" d="M183.62,-262.9C176.08,-247.78 163.04,-221.65 155.55,-206.63"/>
</g>
<!-- 28&#45;&#45;34 -->
<g id="edge69" class="edge">
<title>28&#45;&#45;34</title>
<path fill="none" stroke="black" d="M177.81,-107.41C171.69,-126.56 159.68,-164.18 153.57,-183.32"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge70" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M209.58,-133.5C212.77,-130.64 216.23,-127.53 219.4,-124.68"/>
</g>
<!-- 29&#45;&#45;34 -->
<g id="edge71" class="edge">
<title>29&#45;&#45;34</title>
<path fill="none" stroke="black" d="M188.53,-154.02C180.11,-162.96 168.03,-175.77 159.61,-184.7"/>
</g>
<!-- 33&#45;&#45;34 -->
<g id="edge78" class="edge">
<title>33&#45;&#45;34</title>
<path fill="none" stroke="black" d="M149.38,-195.26C149.52,-195.21 149.62,-195.17 149.69,-195.15"/>
</g>
<!-- 15 -->
<g id="node25" class="node">
<title>15</title>
<ellipse fill="none" stroke="black" cx="108.56" cy="-288.27" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="108.56" y="-286.37" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge47" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M111.91,-276.18C116.51,-259.57 124.73,-229.87 129.32,-213.31"/>
</g>
<!-- 15&#45;&#45;34 -->
<g id="edge48" class="edge">
<title>15&#45;&#45;34</title>
<path fill="none" stroke="black" d="M113.78,-276.47C121.72,-258.55 136.71,-224.67 144.62,-206.81"/>
</g>
<!-- 16 -->
<g id="node26" class="node">
<title>16</title>
<ellipse fill="none" stroke="black" cx="35.85" cy="-220.46" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="35.85" y="-218.56" font-family="Times,serif" font-size="8.00">16</text>
</g>
<!-- 16&#45;&#45;33 -->
<g id="edge49" class="edge">
<title>16&#45;&#45;33</title>
<path fill="none" stroke="black" d="M54.51,-216.77C71.76,-213.36 97.23,-208.32 114.36,-204.94"/>
</g>
<!-- 16&#45;&#45;34 -->
<g id="edge50" class="edge">
<title>16&#45;&#45;34</title>
<path fill="none" stroke="black" d="M53.9,-216.45C75.12,-211.73 110.12,-203.94 131.45,-199.19"/>
</g>
<!-- 19 -->
<g id="node27" class="node">
<title>19</title>
<ellipse fill="none" stroke="black" cx="70.93" cy="-277.41" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="70.93" y="-275.51" font-family="Times,serif" font-size="8.00">19</text>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge51" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M79.77,-266.51C91.5,-252.05 112.09,-226.66 123.81,-212.21"/>
</g>
<!-- 19&#45;&#45;34 -->
<g id="edge52" class="edge">
<title>19&#45;&#45;34</title>
<path fill="none" stroke="black" d="M80.92,-266.99C96.09,-251.16 124.77,-221.23 139.89,-205.45"/>
</g>
<!-- 21 -->
<g id="node28" class="node">
<title>21</title>
<ellipse fill="none" stroke="black" cx="62.73" cy="-242.96" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="62.73" y="-241.06" font-family="Times,serif" font-size="8.00">21</text>
</g>
<!-- 21&#45;&#45;33 -->
<g id="edge54" class="edge">
<title>21&#45;&#45;33</title>
<path fill="none" stroke="black" d="M76.87,-234.54C89,-227.31 106.39,-216.96 118.51,-209.74"/>
</g>
<!-- 21&#45;&#45;34 -->
<g id="edge55" class="edge">
<title>21&#45;&#45;34</title>
<path fill="none" stroke="black" d="M77.5,-234.85C93.61,-225.99 119.23,-211.91 135.25,-203.1"/>
</g>
<!-- 23 -->
<g id="node29" class="node">
<title>23</title>
<ellipse fill="none" stroke="black" cx="50.98" cy="-185.13" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="50.98" y="-183.23" font-family="Times,serif" font-size="8.00">23</text>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge56" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M69.51,-188.81C82.87,-191.46 100.83,-195.02 114.18,-197.67"/>
</g>
<!-- 23&#45;&#45;34 -->
<g id="edge57" class="edge">
<title>23&#45;&#45;34</title>
<path fill="none" stroke="black" d="M70.04,-187.06C87.53,-188.82 113.33,-191.43 130.81,-193.19"/>
</g>
<!-- 24 -->
<g id="node30" class="node">
<title>24</title>
<ellipse fill="none" stroke="black" cx="114.75" cy="-103.62" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="114.75" y="-101.72" font-family="Times,serif" font-size="8.00">24</text>
</g>
<!-- 24&#45;&#45;28 -->
<g id="edge59" class="edge">
<title>24&#45;&#45;28</title>
<path fill="none" stroke="black" d="M133.72,-101.34C142.83,-100.25 153.7,-98.95 162.79,-97.86"/>
</g>
<!-- 24&#45;&#45;33 -->
<g id="edge61" class="edge">
<title>24&#45;&#45;33</title>
<path fill="none" stroke="black" d="M116.94,-115.6C120.37,-134.35 126.98,-170.43 130.42,-189.25"/>
</g>
<!-- 24&#45;&#45;34 -->
<g id="edge62" class="edge">
<title>24&#45;&#45;34</title>
<path fill="none" stroke="black" d="M119.33,-115.58C126.05,-133.11 138.5,-165.61 145.22,-183.14"/>
</g>
<!-- 26 -->
<g id="node31" class="node">
<title>26</title>
<ellipse fill="none" stroke="black" cx="153.98" cy="-23.11" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="153.98" y="-21.21" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 24&#45;&#45;26 -->
<g id="edge58" class="edge">
<title>24&#45;&#45;26</title>
<path fill="none" stroke="black" d="M120.37,-92.08C127.82,-76.78 140.91,-49.92 148.37,-34.63"/>
</g>
<!-- 30 -->
<g id="node32" class="node">
<title>30</title>
<ellipse fill="none" stroke="black" cx="69.77" cy="-137.42" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="69.77" y="-135.52" font-family="Times,serif" font-size="8.00">30</text>
</g>
<!-- 24&#45;&#45;30 -->
<g id="edge60" class="edge">
<title>24&#45;&#45;30</title>
<path fill="none" stroke="black" d="M102.46,-112.85C96.11,-117.62 88.39,-123.43 82.05,-128.2"/>
</g>
<!-- 26&#45;&#45;32 -->
<g id="edge66" class="edge">
<title>26&#45;&#45;32</title>
<path fill="none" stroke="black" d="M163.05,-33.98C177.77,-51.62 206.69,-86.25 221.4,-103.88"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge72" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M79.87,-147.69C91.53,-159.54 110.61,-178.94 122.35,-190.87"/>
</g>
<!-- 30&#45;&#45;34 -->
<g id="edge73" class="edge">
<title>30&#45;&#45;34</title>
<path fill="none" stroke="black" d="M82.63,-146.69C97.65,-157.52 122.35,-175.32 137.24,-186.05"/>
</g>
<!-- 25 -->
<g id="node33" class="node">
<title>25</title>
<ellipse fill="none" stroke="black" cx="202.68" cy="-12.02" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="202.68" y="-10.12" font-family="Times,serif" font-size="8.00">25</text>
</g>
<!-- 25&#45;&#45;32 -->
<g id="edge65" class="edge">
<title>25&#45;&#45;32</title>
<path fill="none" stroke="black" d="M205.98,-24.21C211.35,-44.08 221.94,-83.22 227.25,-102.88"/>
</g>
<!-- 25&#45;&#45;28 -->
<g id="edge64" class="edge">
<title>25&#45;&#45;28</title>
<path fill="none" stroke="black" d="M199.66,-24C195.65,-39.89 188.6,-67.78 184.59,-83.66"/>
</g>
<!-- 25&#45;&#45;26 -->
<g id="edge63" class="edge">
<title>25&#45;&#45;26</title>
<path fill="none" stroke="black" d="M184.55,-16.15C180.44,-17.09 176.07,-18.08 171.97,-19.02"/>
</g>
<!-- 27 -->
<g id="node34" class="node">
<title>27</title>
<ellipse fill="none" stroke="black" cx="19.09" cy="-136.06" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="19.09" y="-134.16" font-family="Times,serif" font-size="8.00">27</text>
</g>
<!-- 27&#45;&#45;34 -->
<g id="edge68" class="edge">
<title>27&#45;&#45;34</title>
<path fill="none" stroke="black" d="M34.86,-143.18C59.98,-154.53 108.82,-176.59 133.98,-187.96"/>
</g>
<!-- 27&#45;&#45;30 -->
<g id="edge67" class="edge">
<title>27&#45;&#45;30</title>
<path fill="none" stroke="black" d="M38.25,-136.57C42.29,-136.68 46.55,-136.8 50.58,-136.91"/>
</g>
</g>
</svg>
</div>


## BFS

The core operation of Graph Algorithms is Breadth First Search, or
BFS.  In the GraphBLAS, this pattern usually consists of vector
matrix multiplication in a loop until some terminal condition is
reached.

Min Parent BFS returns a sparse vector of nodes containing the
minimum parent id:
``` postgres-console
CREATE OR REPLACE FUNCTION bfs(
        graph matrix,          -- This is the input graph
        start_node bigint      -- This is the node id to start the BFS
    )
    RETURNS vector             -- will return a vector of (node:minparent)
    LANGUAGE plpgsql AS
    $$
    DECLARE
    bfs_vector vector = vector('int64');  -- The result vector to accumulate ids
    prev_vector vector = vector('int64'); -- temp vector to detect termination
    BEGIN
        bfs_vector = set_element(bfs_vector, start_node, 1); -- set the start node value to 1
        WHILE bfs_vector != prev_vector LOOP  -- keep looping when the result changes
            prev_vector = dup(bfs_vector);    -- dup bfs and assign to previous
            bfs_vector = vxm(
                bfs_vector,          -- Starting from the current frontier
                graph,               -- traverse this graph.
                'any_secondi_int64', -- At any edge value, store the parent node id
                c=>bfs_vector,       -- assign back into bfs_vector
                accum=>'min_int64'   -- accumulate the minimum parent id for collisions
            );
        END LOOP;
    RETURN bfs_vector;
    end;
    $$;
```
Now run the function passing a graph and a starting point:
```
```

<div>
<!-- Title: %3 Pages: 1 -->
<svg width="638pt" height="305pt"
 viewBox="0.00 0.00 637.87 304.84" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 300.84)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-300.84 633.87,-300.84 633.87,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="251.89" cy="-122.11" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="251.89" y="-120.21" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="308.2" cy="-63.55" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="308.2" y="-61.65" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge1" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M261.19,-112.44C271.56,-101.65 288.25,-84.3 298.7,-73.43"/>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="374.5" cy="-104.01" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="374.5" y="-102.11" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge2" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M267.17,-119.85C290.67,-116.38 335.42,-109.78 359.03,-106.3"/>
</g>
<!-- 4 -->
<g id="node4" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="274.77" cy="-58.97" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="274.77" y="-57.07" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;4 -->
<g id="edge3" class="edge">
<title>1&#45;&#45;4</title>
<path fill="none" stroke="black" d="M256.08,-110.53C260.16,-99.28 266.32,-82.27 270.44,-70.9"/>
</g>
<!-- 5 -->
<g id="node5" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="145.96" cy="-128.93" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="145.96" y="-127.03" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;5 -->
<g id="edge4" class="edge">
<title>1&#45;&#45;5</title>
<path fill="none" stroke="black" d="M236.26,-123.11C216.23,-124.41 181.71,-126.63 161.65,-127.92"/>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="103.58" cy="-133.99" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="103.58" y="-132.09" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;6 -->
<g id="edge5" class="edge">
<title>1&#45;&#45;6</title>
<path fill="none" stroke="black" d="M236.3,-123.36C207.89,-125.63 147.99,-130.43 119.41,-132.72"/>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="107.49" cy="-163.71" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="107.49" y="-161.81" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge6" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M237.25,-126.33C209.57,-134.3 149.87,-151.5 122.17,-159.48"/>
</g>
<!-- 9 -->
<g id="node8" class="node">
<title>9</title>
<ellipse fill="none" stroke="black" cx="403.2" cy="-102.9" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="403.2" y="-101" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;9 -->
<g id="edge8" class="edge">
<title>1&#45;&#45;9</title>
<path fill="none" stroke="black" d="M267.51,-120.12C296.51,-116.44 358.34,-108.59 387.44,-104.9"/>
</g>
<!-- 14 -->
<g id="node10" class="node">
<title>14</title>
<ellipse fill="none" stroke="black" cx="350.93" cy="-74.04" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="350.93" y="-72.14" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;14 -->
<g id="edge12" class="edge">
<title>1&#45;&#45;14</title>
<path fill="none" stroke="black" d="M265.24,-115.63C284.15,-106.45 318.51,-89.78 337.47,-80.58"/>
</g>
<!-- 20 -->
<g id="node14" class="node">
<title>20</title>
<ellipse fill="none" stroke="black" cx="350.76" cy="-133.23" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="350.76" y="-131.33" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;20 -->
<g id="edge14" class="edge">
<title>1&#45;&#45;20</title>
<path fill="none" stroke="black" d="M267.34,-123.85C285.8,-125.92 316.53,-129.38 335.1,-131.47"/>
</g>
<!-- 32 -->
<g id="node25" class="node">
<title>32</title>
<ellipse fill="none" stroke="black" cx="412.89" cy="-189.67" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="412.89" y="-187.77" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 1&#45;&#45;32 -->
<g id="edge16" class="edge">
<title>1&#45;&#45;32</title>
<path fill="none" stroke="black" d="M265.62,-127.87C296.03,-140.63 368.81,-171.17 399.2,-183.92"/>
</g>
<!-- 8 -->
<g id="node27" class="node">
<title>8</title>
<ellipse fill="none" stroke="black" cx="288.33" cy="-105.22" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="288.33" y="-103.32" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&#45;8 -->
<g id="edge7" class="edge">
<title>1&#45;&#45;8</title>
<path fill="none" stroke="black" d="M265.46,-115.82C268.53,-114.39 271.8,-112.88 274.87,-111.46"/>
</g>
<!-- 11 -->
<g id="node28" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="152.51" cy="-170.25" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="152.51" y="-168.35" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 1&#45;&#45;11 -->
<g id="edge9" class="edge">
<title>1&#45;&#45;11</title>
<path fill="none" stroke="black" d="M238.49,-128.6C220.05,-137.53 186.96,-153.56 167.65,-162.91"/>
</g>
<!-- 12 -->
<g id="node29" class="node">
<title>12</title>
<ellipse fill="none" stroke="black" cx="196.06" cy="-228.22" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="196.06" y="-226.32" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1&#45;&#45;12 -->
<g id="edge10" class="edge">
<title>1&#45;&#45;12</title>
<path fill="none" stroke="black" d="M245.91,-133.46C235.22,-153.78 212.94,-196.14 202.16,-216.63"/>
</g>
<!-- 13 -->
<g id="node30" class="node">
<title>13</title>
<ellipse fill="none" stroke="black" cx="186.28" cy="-34.17" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="186.28" y="-32.27" font-family="Times,serif" font-size="8.00">13</text>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge11" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M244.1,-111.68C231.61,-94.93 207.21,-62.22 194.47,-45.14"/>
</g>
<!-- 18 -->
<g id="node31" class="node">
<title>18</title>
<ellipse fill="none" stroke="black" cx="214.24" cy="-61.72" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="214.24" y="-59.82" font-family="Times,serif" font-size="8.00">18</text>
</g>
<!-- 1&#45;&#45;18 -->
<g id="edge13" class="edge">
<title>1&#45;&#45;18</title>
<path fill="none" stroke="black" d="M244.98,-111.03C238.28,-100.27 228.14,-84.01 221.36,-73.14"/>
</g>
<!-- 22 -->
<g id="node32" class="node">
<title>22</title>
<ellipse fill="none" stroke="black" cx="238.71" cy="-12.02" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="238.71" y="-10.12" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&#45;22 -->
<g id="edge15" class="edge">
<title>1&#45;&#45;22</title>
<path fill="none" stroke="black" d="M250.43,-109.91C247.89,-88.74 242.72,-45.48 240.18,-24.28"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge17" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M320.67,-71.16C332.53,-78.39 350.25,-89.21 362.09,-96.44"/>
</g>
<!-- 2&#45;&#45;4 -->
<g id="edge18" class="edge">
<title>2&#45;&#45;4</title>
<path fill="none" stroke="black" d="M292.66,-61.42C291.84,-61.31 291.02,-61.19 290.2,-61.08"/>
</g>
<!-- 2&#45;&#45;14 -->
<g id="edge20" class="edge">
<title>2&#45;&#45;14</title>
<path fill="none" stroke="black" d="M323.15,-67.22C327.28,-68.24 331.78,-69.34 335.92,-70.36"/>
</g>
<!-- 2&#45;&#45;20 -->
<g id="edge22" class="edge">
<title>2&#45;&#45;20</title>
<path fill="none" stroke="black" d="M314.85,-74.44C322.8,-87.45 336.03,-109.11 344.02,-122.19"/>
</g>
<!-- 31 -->
<g id="node24" class="node">
<title>31</title>
<ellipse fill="none" stroke="black" cx="426.18" cy="-59.54" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="426.18" y="-57.64" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 2&#45;&#45;31 -->
<g id="edge24" class="edge">
<title>2&#45;&#45;31</title>
<path fill="none" stroke="black" d="M324.11,-63.01C346.76,-62.24 388.02,-60.84 410.52,-60.07"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge19" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M302.77,-74.93C299.97,-80.81 296.56,-87.97 293.76,-93.84"/>
</g>
<!-- 2&#45;&#45;18 -->
<g id="edge21" class="edge">
<title>2&#45;&#45;18</title>
<path fill="none" stroke="black" d="M292.26,-63.24C276.03,-62.92 250.86,-62.43 233.52,-62.09"/>
</g>
<!-- 2&#45;&#45;22 -->
<g id="edge23" class="edge">
<title>2&#45;&#45;22</title>
<path fill="none" stroke="black" d="M296.73,-55.04C284.24,-45.78 264.32,-31.01 251.38,-21.42"/>
</g>
<!-- 3&#45;&#45;4 -->
<g id="edge25" class="edge">
<title>3&#45;&#45;4</title>
<path fill="none" stroke="black" d="M361.05,-97.94C342.02,-89.34 307.41,-73.71 288.32,-65.09"/>
</g>
<!-- 3&#45;&#45;9 -->
<g id="edge27" class="edge">
<title>3&#45;&#45;9</title>
<path fill="none" stroke="black" d="M390.2,-103.4C390.3,-103.4 390.41,-103.39 390.51,-103.39"/>
</g>
<!-- 10 -->
<g id="node9" class="node">
<title>10</title>
<ellipse fill="none" stroke="black" cx="449.79" cy="-25.51" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="449.79" y="-23.61" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3&#45;&#45;10 -->
<g id="edge28" class="edge">
<title>3&#45;&#45;10</title>
<path fill="none" stroke="black" d="M384.04,-94.07C398.52,-78.97 425.89,-50.42 440.33,-35.37"/>
</g>
<!-- 3&#45;&#45;14 -->
<g id="edge29" class="edge">
<title>3&#45;&#45;14</title>
<path fill="none" stroke="black" d="M366.26,-93.53C363.92,-90.56 361.38,-87.33 359.05,-84.37"/>
</g>
<!-- 28 -->
<g id="node21" class="node">
<title>28</title>
<ellipse fill="none" stroke="black" cx="457.01" cy="-200.43" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="457.01" y="-198.53" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3&#45;&#45;28 -->
<g id="edge30" class="edge">
<title>3&#45;&#45;28</title>
<path fill="none" stroke="black" d="M383.33,-114.33C399.21,-132.89 432.41,-171.69 448.24,-190.19"/>
</g>
<!-- 29 -->
<g id="node22" class="node">
<title>29</title>
<ellipse fill="none" stroke="black" cx="438.35" cy="-152.94" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="438.35" y="-151.04" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3&#45;&#45;29 -->
<g id="edge31" class="edge">
<title>3&#45;&#45;29</title>
<path fill="none" stroke="black" d="M385.62,-112.53C397.2,-121.41 415.24,-135.23 426.92,-144.19"/>
</g>
<!-- 33 -->
<g id="node26" class="node">
<title>33</title>
<ellipse fill="none" stroke="black" cx="506.38" cy="-104.21" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="506.38" y="-102.31" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 3&#45;&#45;33 -->
<g id="edge32" class="edge">
<title>3&#45;&#45;33</title>
<path fill="none" stroke="black" d="M390.15,-104.04C415.53,-104.07 465.39,-104.15 490.76,-104.18"/>
</g>
<!-- 3&#45;&#45;8 -->
<g id="edge26" class="edge">
<title>3&#45;&#45;8</title>
<path fill="none" stroke="black" d="M358.7,-104.23C343.15,-104.45 319.55,-104.78 304.04,-105"/>
</g>
<!-- 4&#45;&#45;14 -->
<g id="edge35" class="edge">
<title>4&#45;&#45;14</title>
<path fill="none" stroke="black" d="M290.18,-62.02C303.48,-64.65 322.58,-68.43 335.8,-71.05"/>
</g>
<!-- 4&#45;&#45;8 -->
<g id="edge33" class="edge">
<title>4&#45;&#45;8</title>
<path fill="none" stroke="black" d="M278.33,-71.12C280.34,-77.97 282.84,-86.49 284.84,-93.31"/>
</g>
<!-- 4&#45;&#45;13 -->
<g id="edge34" class="edge">
<title>4&#45;&#45;13</title>
<path fill="none" stroke="black" d="M259.76,-54.76C244.27,-50.42 220.15,-43.66 203.8,-39.08"/>
</g>
<!-- 5&#45;&#45;7 -->
<g id="edge36" class="edge">
<title>5&#45;&#45;7</title>
<path fill="none" stroke="black" d="M135.45,-138.44C129.93,-143.42 123.2,-149.51 117.72,-154.46"/>
</g>
<!-- 5&#45;&#45;11 -->
<g id="edge37" class="edge">
<title>5&#45;&#45;11</title>
<path fill="none" stroke="black" d="M147.89,-141.1C148.73,-146.44 149.72,-152.68 150.57,-158.02"/>
</g>
<!-- 6&#45;&#45;7 -->
<g id="edge38" class="edge">
<title>6&#45;&#45;7</title>
<path fill="none" stroke="black" d="M105.17,-146.08C105.41,-147.88 105.65,-149.73 105.89,-151.53"/>
</g>
<!-- 6&#45;&#45;11 -->
<g id="edge39" class="edge">
<title>6&#45;&#45;11</title>
<path fill="none" stroke="black" d="M114.93,-142.4C122.38,-147.92 132.14,-155.15 139.85,-160.86"/>
</g>
<!-- 17 -->
<g id="node33" class="node">
<title>17</title>
<ellipse fill="none" stroke="black" cx="19.09" cy="-163.34" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="19.09" y="-161.44" font-family="Times,serif" font-size="8.00">17</text>
</g>
<!-- 6&#45;&#45;17 -->
<g id="edge40" class="edge">
<title>6&#45;&#45;17</title>
<path fill="none" stroke="black" d="M89.25,-138.97C74.47,-144.1 51.43,-152.11 35.82,-157.53"/>
</g>
<!-- 7&#45;&#45;17 -->
<g id="edge41" class="edge">
<title>7&#45;&#45;17</title>
<path fill="none" stroke="black" d="M91.68,-163.64C76.79,-163.58 54.42,-163.49 38.46,-163.42"/>
</g>
<!-- 9&#45;&#45;31 -->
<g id="edge42" class="edge">
<title>9&#45;&#45;31</title>
<path fill="none" stroke="black" d="M409.24,-91.51C412.64,-85.08 416.88,-77.09 420.26,-70.71"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge43" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M418.88,-103.1C438.27,-103.34 471.1,-103.76 490.57,-104.01"/>
</g>
<!-- 34 -->
<g id="node34" class="node">
<title>34</title>
<ellipse fill="none" stroke="black" cx="489.86" cy="-114.16" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="489.86" y="-112.26" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 9&#45;&#45;34 -->
<g id="edge44" class="edge">
<title>9&#45;&#45;34</title>
<path fill="none" stroke="black" d="M418.7,-104.91C433.3,-106.81 455.22,-109.66 470.87,-111.69"/>
</g>
<!-- 10&#45;&#45;34 -->
<g id="edge45" class="edge">
<title>10&#45;&#45;34</title>
<path fill="none" stroke="black" d="M455.03,-37.1C462.71,-54.09 476.94,-85.58 484.62,-102.57"/>
</g>
<!-- 14&#45;&#45;34 -->
<g id="edge46" class="edge">
<title>14&#45;&#45;34</title>
<path fill="none" stroke="black" d="M365.53,-78.26C391.31,-85.7 444.76,-101.14 472.42,-109.12"/>
</g>
<!-- 15 -->
<g id="node11" class="node">
<title>15</title>
<ellipse fill="none" stroke="black" cx="530.08" cy="-23.47" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="530.08" y="-21.57" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge47" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M526.58,-35.39C522.1,-50.65 514.38,-76.96 509.89,-92.25"/>
</g>
<!-- 15&#45;&#45;34 -->
<g id="edge48" class="edge">
<title>15&#45;&#45;34</title>
<path fill="none" stroke="black" d="M524.98,-34.96C517.29,-52.31 502.78,-85.02 495.04,-102.48"/>
</g>
<!-- 16 -->
<g id="node12" class="node">
<title>16</title>
<ellipse fill="none" stroke="black" cx="599.63" cy="-89.53" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="599.63" y="-87.63" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 16&#45;&#45;33 -->
<g id="edge49" class="edge">
<title>16&#45;&#45;33</title>
<path fill="none" stroke="black" d="M584.23,-91.95C566.96,-94.67 539.09,-99.06 521.8,-101.78"/>
</g>
<!-- 16&#45;&#45;34 -->
<g id="edge50" class="edge">
<title>16&#45;&#45;34</title>
<path fill="none" stroke="black" d="M584.37,-92.95C564.34,-97.45 529.24,-105.32 507.91,-110.11"/>
</g>
<!-- 19 -->
<g id="node13" class="node">
<title>19</title>
<ellipse fill="none" stroke="black" cx="565.94" cy="-34.12" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="565.94" y="-32.22" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge51" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M557.15,-44.46C545.89,-57.71 526.48,-80.55 515.21,-93.82"/>
</g>
<!-- 19&#45;&#45;34 -->
<g id="edge52" class="edge">
<title>19&#45;&#45;34</title>
<path fill="none" stroke="black" d="M556.61,-43.93C542.17,-59.13 514.52,-88.21 499.75,-103.76"/>
</g>
<!-- 20&#45;&#45;34 -->
<g id="edge53" class="edge">
<title>20&#45;&#45;34</title>
<path fill="none" stroke="black" d="M366.18,-131.12C391.79,-127.61 443.01,-120.58 470.83,-116.77"/>
</g>
<!-- 21 -->
<g id="node15" class="node">
<title>21</title>
<ellipse fill="none" stroke="black" cx="574.77" cy="-67" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="574.77" y="-65.1" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 21&#45;&#45;33 -->
<g id="edge54" class="edge">
<title>21&#45;&#45;33</title>
<path fill="none" stroke="black" d="M561.9,-73.99C549.68,-80.65 531.39,-90.6 519.19,-97.24"/>
</g>
<!-- 21&#45;&#45;34 -->
<g id="edge55" class="edge">
<title>21&#45;&#45;34</title>
<path fill="none" stroke="black" d="M561.87,-74.16C546.38,-82.77 520.45,-97.16 504.3,-106.14"/>
</g>
<!-- 23 -->
<g id="node16" class="node">
<title>23</title>
<ellipse fill="none" stroke="black" cx="584.88" cy="-122.94" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="584.88" y="-121.04" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge56" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M569.75,-119.33C555.85,-116.01 535.36,-111.12 521.47,-107.81"/>
</g>
<!-- 23&#45;&#45;34 -->
<g id="edge57" class="edge">
<title>23&#45;&#45;34</title>
<path fill="none" stroke="black" d="M569.19,-121.49C552.64,-119.96 526.55,-117.55 508.85,-115.91"/>
</g>
<!-- 24 -->
<g id="node17" class="node">
<title>24</title>
<ellipse fill="none" stroke="black" cx="522.57" cy="-199.5" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="522.57" y="-197.6" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 26 -->
<g id="node19" class="node">
<title>26</title>
<ellipse fill="none" stroke="black" cx="485.91" cy="-276.78" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="485.91" y="-274.88" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 24&#45;&#45;26 -->
<g id="edge58" class="edge">
<title>24&#45;&#45;26</title>
<path fill="none" stroke="black" d="M517.16,-210.91C510.23,-225.52 498.28,-250.7 491.34,-265.33"/>
</g>
<!-- 24&#45;&#45;28 -->
<g id="edge59" class="edge">
<title>24&#45;&#45;28</title>
<path fill="none" stroke="black" d="M506.7,-199.73C496.39,-199.88 483.01,-200.07 472.73,-200.21"/>
</g>
<!-- 30 -->
<g id="node23" class="node">
<title>30</title>
<ellipse fill="none" stroke="black" cx="566.47" cy="-167.87" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="566.47" y="-165.97" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 24&#45;&#45;30 -->
<g id="edge60" class="edge">
<title>24&#45;&#45;30</title>
<path fill="none" stroke="black" d="M534.1,-191.19C540.51,-186.58 548.45,-180.86 554.86,-176.24"/>
</g>
<!-- 24&#45;&#45;33 -->
<g id="edge61" class="edge">
<title>24&#45;&#45;33</title>
<path fill="none" stroke="black" d="M520.52,-187.43C517.41,-169.1 511.52,-134.45 508.42,-116.18"/>
</g>
<!-- 24&#45;&#45;34 -->
<g id="edge62" class="edge">
<title>24&#45;&#45;34</title>
<path fill="none" stroke="black" d="M518.02,-187.64C511.77,-171.33 500.6,-142.18 494.37,-125.92"/>
</g>
<!-- 25 -->
<g id="node18" class="node">
<title>25</title>
<ellipse fill="none" stroke="black" cx="439.36" cy="-284.82" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="439.36" y="-282.92" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 25&#45;&#45;26 -->
<g id="edge63" class="edge">
<title>25&#45;&#45;26</title>
<path fill="none" stroke="black" d="M454.6,-282.19C459.69,-281.31 465.37,-280.33 470.47,-279.45"/>
</g>
<!-- 25&#45;&#45;28 -->
<g id="edge64" class="edge">
<title>25&#45;&#45;28</title>
<path fill="none" stroke="black" d="M441.88,-272.73C445.24,-256.69 451.13,-228.53 454.49,-212.5"/>
</g>
<!-- 25&#45;&#45;32 -->
<g id="edge65" class="edge">
<title>25&#45;&#45;32</title>
<path fill="none" stroke="black" d="M436,-272.77C430.91,-254.46 421.29,-219.86 416.22,-201.62"/>
</g>
<!-- 26&#45;&#45;32 -->
<g id="edge66" class="edge">
<title>26&#45;&#45;32</title>
<path fill="none" stroke="black" d="M477.25,-266.45C463.19,-249.68 435.59,-216.75 421.54,-199.99"/>
</g>
<!-- 27 -->
<g id="node20" class="node">
<title>27</title>
<ellipse fill="none" stroke="black" cx="614.32" cy="-172.85" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="614.32" y="-170.95" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 27&#45;&#45;30 -->
<g id="edge67" class="edge">
<title>27&#45;&#45;30</title>
<path fill="none" stroke="black" d="M598.65,-171.22C593.31,-170.66 587.34,-170.04 582.01,-169.49"/>
</g>
<!-- 27&#45;&#45;34 -->
<g id="edge68" class="edge">
<title>27&#45;&#45;34</title>
<path fill="none" stroke="black" d="M601,-166.57C577.8,-155.62 530.09,-133.13 505.41,-121.49"/>
</g>
<!-- 28&#45;&#45;34 -->
<g id="edge69" class="edge">
<title>28&#45;&#45;34</title>
<path fill="none" stroke="black" d="M461.44,-188.8C467.71,-172.33 479.11,-142.4 485.4,-125.89"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge70" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M430.85,-163.76C427.56,-168.5 423.71,-174.05 420.43,-178.8"/>
</g>
<!-- 29&#45;&#45;34 -->
<g id="edge71" class="edge">
<title>29&#45;&#45;34</title>
<path fill="none" stroke="black" d="M449.52,-144.53C457.69,-138.38 468.77,-130.03 477.26,-123.65"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge72" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M557.08,-157.92C545.86,-146.03 527.19,-126.25 515.9,-114.29"/>
</g>
<!-- 30&#45;&#45;34 -->
<g id="edge73" class="edge">
<title>30&#45;&#45;34</title>
<path fill="none" stroke="black" d="M554.84,-159.71C540.77,-149.85 517.18,-133.31 502.63,-123.11"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge74" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M439.07,-66.72C454.03,-75.05 478.58,-88.72 493.53,-97.05"/>
</g>
<!-- 31&#45;&#45;34 -->
<g id="edge75" class="edge">
<title>31&#45;&#45;34</title>
<path fill="none" stroke="black" d="M436.7,-68.56C448.14,-78.38 466.39,-94.03 478.25,-104.2"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge76" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M423.25,-180.2C441.32,-163.68 478.36,-129.82 496.26,-113.46"/>
</g>
<!-- 32&#45;&#45;34 -->
<g id="edge77" class="edge">
<title>32&#45;&#45;34</title>
<path fill="none" stroke="black" d="M422.64,-180.1C437.2,-165.82 464.52,-139.02 479.46,-124.37"/>
</g>
<!-- 33&#45;&#45;34 -->
<g id="edge78" class="edge">
<title>33&#45;&#45;34</title>
<path fill="none" stroke="black" d="M494.03,-111.65C493.88,-111.74 493.73,-111.83 493.59,-111.91"/>
</g>
</g>
</svg>
</div>


## SSSP

The Single Source [Shortest
Path](https://en.wikipedia.org/wiki/Shortest_path_problem)
algorithm is a varation on BFS with a different semiring called
`min_plus`.  Instead of taking the minium parent id, the semiring
takes the minimum path length from a single source to every node.
This implies adding path lengths instead of multiplying them as
would happen in the common `plus_times` semiring.  In this example
we use one of OneSparse experimental operators `@<+<` to specify an
in place matrix multiplication and accumulation with the `min_plus`
semiring (`@<+`) and the minimum accumulator `<` combine to form
`@<+<`.

``` postgres-console
CREATE OR REPLACE FUNCTION sssp(graph matrix, start_node bigint)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
    sssp_vector vector = vector(type(graph));
    next_vector vector = vector(type(graph));
    BEGIN
        sssp_vector = set_element(sssp_vector, start_node, 1);
        WHILE sssp_vector != next_vector LOOP
            next_vector = dup(sssp_vector);
            sssp_vector = sssp_vector @<+< graph;
        END LOOP;
    RETURN sssp_vector;
    end;
    $$;
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="638pt" height="305pt"
 viewBox="0.00 0.00 637.87 304.84" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 300.84)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-300.84 633.87,-300.84 633.87,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="251.89" cy="-122.11" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="251.89" y="-120.21" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="308.2" cy="-63.55" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="308.2" y="-61.65" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge1" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M261.19,-112.44C271.56,-101.65 288.25,-84.3 298.7,-73.43"/>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="374.5" cy="-104.01" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="374.5" y="-102.11" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge2" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M267.17,-119.85C290.67,-116.38 335.42,-109.78 359.03,-106.3"/>
</g>
<!-- 4 -->
<g id="node4" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="274.77" cy="-58.97" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="274.77" y="-57.07" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;4 -->
<g id="edge3" class="edge">
<title>1&#45;&#45;4</title>
<path fill="none" stroke="black" d="M256.08,-110.53C260.16,-99.28 266.32,-82.27 270.44,-70.9"/>
</g>
<!-- 5 -->
<g id="node5" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="145.96" cy="-128.93" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="145.96" y="-127.03" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;5 -->
<g id="edge4" class="edge">
<title>1&#45;&#45;5</title>
<path fill="none" stroke="black" d="M236.26,-123.11C216.23,-124.41 181.71,-126.63 161.65,-127.92"/>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="103.58" cy="-133.99" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="103.58" y="-132.09" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;6 -->
<g id="edge5" class="edge">
<title>1&#45;&#45;6</title>
<path fill="none" stroke="black" d="M236.3,-123.36C207.89,-125.63 147.99,-130.43 119.41,-132.72"/>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="107.49" cy="-163.71" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="107.49" y="-161.81" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge6" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M237.25,-126.33C209.57,-134.3 149.87,-151.5 122.17,-159.48"/>
</g>
<!-- 9 -->
<g id="node8" class="node">
<title>9</title>
<ellipse fill="none" stroke="black" cx="403.2" cy="-102.9" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="403.2" y="-101" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;9 -->
<g id="edge8" class="edge">
<title>1&#45;&#45;9</title>
<path fill="none" stroke="black" d="M267.51,-120.12C296.51,-116.44 358.34,-108.59 387.44,-104.9"/>
</g>
<!-- 14 -->
<g id="node10" class="node">
<title>14</title>
<ellipse fill="none" stroke="black" cx="350.93" cy="-74.04" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="350.93" y="-72.14" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;14 -->
<g id="edge12" class="edge">
<title>1&#45;&#45;14</title>
<path fill="none" stroke="black" d="M265.24,-115.63C284.15,-106.45 318.51,-89.78 337.47,-80.58"/>
</g>
<!-- 20 -->
<g id="node14" class="node">
<title>20</title>
<ellipse fill="none" stroke="black" cx="350.76" cy="-133.23" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="350.76" y="-131.33" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;20 -->
<g id="edge14" class="edge">
<title>1&#45;&#45;20</title>
<path fill="none" stroke="black" d="M267.34,-123.85C285.8,-125.92 316.53,-129.38 335.1,-131.47"/>
</g>
<!-- 32 -->
<g id="node25" class="node">
<title>32</title>
<ellipse fill="none" stroke="black" cx="412.89" cy="-189.67" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="412.89" y="-187.77" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 1&#45;&#45;32 -->
<g id="edge16" class="edge">
<title>1&#45;&#45;32</title>
<path fill="none" stroke="black" d="M265.62,-127.87C296.03,-140.63 368.81,-171.17 399.2,-183.92"/>
</g>
<!-- 8 -->
<g id="node27" class="node">
<title>8</title>
<ellipse fill="none" stroke="black" cx="288.33" cy="-105.22" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="288.33" y="-103.32" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&#45;8 -->
<g id="edge7" class="edge">
<title>1&#45;&#45;8</title>
<path fill="none" stroke="black" d="M265.46,-115.82C268.53,-114.39 271.8,-112.88 274.87,-111.46"/>
</g>
<!-- 11 -->
<g id="node28" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="152.51" cy="-170.25" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="152.51" y="-168.35" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 1&#45;&#45;11 -->
<g id="edge9" class="edge">
<title>1&#45;&#45;11</title>
<path fill="none" stroke="black" d="M238.49,-128.6C220.05,-137.53 186.96,-153.56 167.65,-162.91"/>
</g>
<!-- 12 -->
<g id="node29" class="node">
<title>12</title>
<ellipse fill="none" stroke="black" cx="196.06" cy="-228.22" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="196.06" y="-226.32" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1&#45;&#45;12 -->
<g id="edge10" class="edge">
<title>1&#45;&#45;12</title>
<path fill="none" stroke="black" d="M245.91,-133.46C235.22,-153.78 212.94,-196.14 202.16,-216.63"/>
</g>
<!-- 13 -->
<g id="node30" class="node">
<title>13</title>
<ellipse fill="none" stroke="black" cx="186.28" cy="-34.17" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="186.28" y="-32.27" font-family="Times,serif" font-size="8.00">13</text>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge11" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M244.1,-111.68C231.61,-94.93 207.21,-62.22 194.47,-45.14"/>
</g>
<!-- 18 -->
<g id="node31" class="node">
<title>18</title>
<ellipse fill="none" stroke="black" cx="214.24" cy="-61.72" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="214.24" y="-59.82" font-family="Times,serif" font-size="8.00">18</text>
</g>
<!-- 1&#45;&#45;18 -->
<g id="edge13" class="edge">
<title>1&#45;&#45;18</title>
<path fill="none" stroke="black" d="M244.98,-111.03C238.28,-100.27 228.14,-84.01 221.36,-73.14"/>
</g>
<!-- 22 -->
<g id="node32" class="node">
<title>22</title>
<ellipse fill="none" stroke="black" cx="238.71" cy="-12.02" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="238.71" y="-10.12" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&#45;22 -->
<g id="edge15" class="edge">
<title>1&#45;&#45;22</title>
<path fill="none" stroke="black" d="M250.43,-109.91C247.89,-88.74 242.72,-45.48 240.18,-24.28"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge17" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M320.67,-71.16C332.53,-78.39 350.25,-89.21 362.09,-96.44"/>
</g>
<!-- 2&#45;&#45;4 -->
<g id="edge18" class="edge">
<title>2&#45;&#45;4</title>
<path fill="none" stroke="black" d="M292.66,-61.42C291.84,-61.31 291.02,-61.19 290.2,-61.08"/>
</g>
<!-- 2&#45;&#45;14 -->
<g id="edge20" class="edge">
<title>2&#45;&#45;14</title>
<path fill="none" stroke="black" d="M323.15,-67.22C327.28,-68.24 331.78,-69.34 335.92,-70.36"/>
</g>
<!-- 2&#45;&#45;20 -->
<g id="edge22" class="edge">
<title>2&#45;&#45;20</title>
<path fill="none" stroke="black" d="M314.85,-74.44C322.8,-87.45 336.03,-109.11 344.02,-122.19"/>
</g>
<!-- 31 -->
<g id="node24" class="node">
<title>31</title>
<ellipse fill="none" stroke="black" cx="426.18" cy="-59.54" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="426.18" y="-57.64" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 2&#45;&#45;31 -->
<g id="edge24" class="edge">
<title>2&#45;&#45;31</title>
<path fill="none" stroke="black" d="M324.11,-63.01C346.76,-62.24 388.02,-60.84 410.52,-60.07"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge19" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M302.77,-74.93C299.97,-80.81 296.56,-87.97 293.76,-93.84"/>
</g>
<!-- 2&#45;&#45;18 -->
<g id="edge21" class="edge">
<title>2&#45;&#45;18</title>
<path fill="none" stroke="black" d="M292.26,-63.24C276.03,-62.92 250.86,-62.43 233.52,-62.09"/>
</g>
<!-- 2&#45;&#45;22 -->
<g id="edge23" class="edge">
<title>2&#45;&#45;22</title>
<path fill="none" stroke="black" d="M296.73,-55.04C284.24,-45.78 264.32,-31.01 251.38,-21.42"/>
</g>
<!-- 3&#45;&#45;4 -->
<g id="edge25" class="edge">
<title>3&#45;&#45;4</title>
<path fill="none" stroke="black" d="M361.05,-97.94C342.02,-89.34 307.41,-73.71 288.32,-65.09"/>
</g>
<!-- 3&#45;&#45;9 -->
<g id="edge27" class="edge">
<title>3&#45;&#45;9</title>
<path fill="none" stroke="black" d="M390.2,-103.4C390.3,-103.4 390.41,-103.39 390.51,-103.39"/>
</g>
<!-- 10 -->
<g id="node9" class="node">
<title>10</title>
<ellipse fill="none" stroke="black" cx="449.79" cy="-25.51" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="449.79" y="-23.61" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 3&#45;&#45;10 -->
<g id="edge28" class="edge">
<title>3&#45;&#45;10</title>
<path fill="none" stroke="black" d="M384.04,-94.07C398.52,-78.97 425.89,-50.42 440.33,-35.37"/>
</g>
<!-- 3&#45;&#45;14 -->
<g id="edge29" class="edge">
<title>3&#45;&#45;14</title>
<path fill="none" stroke="black" d="M366.26,-93.53C363.92,-90.56 361.38,-87.33 359.05,-84.37"/>
</g>
<!-- 28 -->
<g id="node21" class="node">
<title>28</title>
<ellipse fill="none" stroke="black" cx="457.01" cy="-200.43" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="457.01" y="-198.53" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 3&#45;&#45;28 -->
<g id="edge30" class="edge">
<title>3&#45;&#45;28</title>
<path fill="none" stroke="black" d="M383.33,-114.33C399.21,-132.89 432.41,-171.69 448.24,-190.19"/>
</g>
<!-- 29 -->
<g id="node22" class="node">
<title>29</title>
<ellipse fill="none" stroke="black" cx="438.35" cy="-152.94" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="438.35" y="-151.04" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 3&#45;&#45;29 -->
<g id="edge31" class="edge">
<title>3&#45;&#45;29</title>
<path fill="none" stroke="black" d="M385.62,-112.53C397.2,-121.41 415.24,-135.23 426.92,-144.19"/>
</g>
<!-- 33 -->
<g id="node26" class="node">
<title>33</title>
<ellipse fill="none" stroke="black" cx="506.38" cy="-104.21" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="506.38" y="-102.31" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 3&#45;&#45;33 -->
<g id="edge32" class="edge">
<title>3&#45;&#45;33</title>
<path fill="none" stroke="black" d="M390.15,-104.04C415.53,-104.07 465.39,-104.15 490.76,-104.18"/>
</g>
<!-- 3&#45;&#45;8 -->
<g id="edge26" class="edge">
<title>3&#45;&#45;8</title>
<path fill="none" stroke="black" d="M358.7,-104.23C343.15,-104.45 319.55,-104.78 304.04,-105"/>
</g>
<!-- 4&#45;&#45;14 -->
<g id="edge35" class="edge">
<title>4&#45;&#45;14</title>
<path fill="none" stroke="black" d="M290.18,-62.02C303.48,-64.65 322.58,-68.43 335.8,-71.05"/>
</g>
<!-- 4&#45;&#45;8 -->
<g id="edge33" class="edge">
<title>4&#45;&#45;8</title>
<path fill="none" stroke="black" d="M278.33,-71.12C280.34,-77.97 282.84,-86.49 284.84,-93.31"/>
</g>
<!-- 4&#45;&#45;13 -->
<g id="edge34" class="edge">
<title>4&#45;&#45;13</title>
<path fill="none" stroke="black" d="M259.76,-54.76C244.27,-50.42 220.15,-43.66 203.8,-39.08"/>
</g>
<!-- 5&#45;&#45;7 -->
<g id="edge36" class="edge">
<title>5&#45;&#45;7</title>
<path fill="none" stroke="black" d="M135.45,-138.44C129.93,-143.42 123.2,-149.51 117.72,-154.46"/>
</g>
<!-- 5&#45;&#45;11 -->
<g id="edge37" class="edge">
<title>5&#45;&#45;11</title>
<path fill="none" stroke="black" d="M147.89,-141.1C148.73,-146.44 149.72,-152.68 150.57,-158.02"/>
</g>
<!-- 6&#45;&#45;7 -->
<g id="edge38" class="edge">
<title>6&#45;&#45;7</title>
<path fill="none" stroke="black" d="M105.17,-146.08C105.41,-147.88 105.65,-149.73 105.89,-151.53"/>
</g>
<!-- 6&#45;&#45;11 -->
<g id="edge39" class="edge">
<title>6&#45;&#45;11</title>
<path fill="none" stroke="black" d="M114.93,-142.4C122.38,-147.92 132.14,-155.15 139.85,-160.86"/>
</g>
<!-- 17 -->
<g id="node33" class="node">
<title>17</title>
<ellipse fill="none" stroke="black" cx="19.09" cy="-163.34" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="19.09" y="-161.44" font-family="Times,serif" font-size="8.00">17</text>
</g>
<!-- 6&#45;&#45;17 -->
<g id="edge40" class="edge">
<title>6&#45;&#45;17</title>
<path fill="none" stroke="black" d="M89.25,-138.97C74.47,-144.1 51.43,-152.11 35.82,-157.53"/>
</g>
<!-- 7&#45;&#45;17 -->
<g id="edge41" class="edge">
<title>7&#45;&#45;17</title>
<path fill="none" stroke="black" d="M91.68,-163.64C76.79,-163.58 54.42,-163.49 38.46,-163.42"/>
</g>
<!-- 9&#45;&#45;31 -->
<g id="edge42" class="edge">
<title>9&#45;&#45;31</title>
<path fill="none" stroke="black" d="M409.24,-91.51C412.64,-85.08 416.88,-77.09 420.26,-70.71"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge43" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M418.88,-103.1C438.27,-103.34 471.1,-103.76 490.57,-104.01"/>
</g>
<!-- 34 -->
<g id="node34" class="node">
<title>34</title>
<ellipse fill="none" stroke="black" cx="489.86" cy="-114.16" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="489.86" y="-112.26" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 9&#45;&#45;34 -->
<g id="edge44" class="edge">
<title>9&#45;&#45;34</title>
<path fill="none" stroke="black" d="M418.7,-104.91C433.3,-106.81 455.22,-109.66 470.87,-111.69"/>
</g>
<!-- 10&#45;&#45;34 -->
<g id="edge45" class="edge">
<title>10&#45;&#45;34</title>
<path fill="none" stroke="black" d="M455.03,-37.1C462.71,-54.09 476.94,-85.58 484.62,-102.57"/>
</g>
<!-- 14&#45;&#45;34 -->
<g id="edge46" class="edge">
<title>14&#45;&#45;34</title>
<path fill="none" stroke="black" d="M365.53,-78.26C391.31,-85.7 444.76,-101.14 472.42,-109.12"/>
</g>
<!-- 15 -->
<g id="node11" class="node">
<title>15</title>
<ellipse fill="none" stroke="black" cx="530.08" cy="-23.47" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="530.08" y="-21.57" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge47" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M526.58,-35.39C522.1,-50.65 514.38,-76.96 509.89,-92.25"/>
</g>
<!-- 15&#45;&#45;34 -->
<g id="edge48" class="edge">
<title>15&#45;&#45;34</title>
<path fill="none" stroke="black" d="M524.98,-34.96C517.29,-52.31 502.78,-85.02 495.04,-102.48"/>
</g>
<!-- 16 -->
<g id="node12" class="node">
<title>16</title>
<ellipse fill="none" stroke="black" cx="599.63" cy="-89.53" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="599.63" y="-87.63" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 16&#45;&#45;33 -->
<g id="edge49" class="edge">
<title>16&#45;&#45;33</title>
<path fill="none" stroke="black" d="M584.23,-91.95C566.96,-94.67 539.09,-99.06 521.8,-101.78"/>
</g>
<!-- 16&#45;&#45;34 -->
<g id="edge50" class="edge">
<title>16&#45;&#45;34</title>
<path fill="none" stroke="black" d="M584.37,-92.95C564.34,-97.45 529.24,-105.32 507.91,-110.11"/>
</g>
<!-- 19 -->
<g id="node13" class="node">
<title>19</title>
<ellipse fill="none" stroke="black" cx="565.94" cy="-34.12" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="565.94" y="-32.22" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge51" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M557.15,-44.46C545.89,-57.71 526.48,-80.55 515.21,-93.82"/>
</g>
<!-- 19&#45;&#45;34 -->
<g id="edge52" class="edge">
<title>19&#45;&#45;34</title>
<path fill="none" stroke="black" d="M556.61,-43.93C542.17,-59.13 514.52,-88.21 499.75,-103.76"/>
</g>
<!-- 20&#45;&#45;34 -->
<g id="edge53" class="edge">
<title>20&#45;&#45;34</title>
<path fill="none" stroke="black" d="M366.18,-131.12C391.79,-127.61 443.01,-120.58 470.83,-116.77"/>
</g>
<!-- 21 -->
<g id="node15" class="node">
<title>21</title>
<ellipse fill="none" stroke="black" cx="574.77" cy="-67" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="574.77" y="-65.1" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 21&#45;&#45;33 -->
<g id="edge54" class="edge">
<title>21&#45;&#45;33</title>
<path fill="none" stroke="black" d="M561.9,-73.99C549.68,-80.65 531.39,-90.6 519.19,-97.24"/>
</g>
<!-- 21&#45;&#45;34 -->
<g id="edge55" class="edge">
<title>21&#45;&#45;34</title>
<path fill="none" stroke="black" d="M561.87,-74.16C546.38,-82.77 520.45,-97.16 504.3,-106.14"/>
</g>
<!-- 23 -->
<g id="node16" class="node">
<title>23</title>
<ellipse fill="none" stroke="black" cx="584.88" cy="-122.94" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="584.88" y="-121.04" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge56" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M569.75,-119.33C555.85,-116.01 535.36,-111.12 521.47,-107.81"/>
</g>
<!-- 23&#45;&#45;34 -->
<g id="edge57" class="edge">
<title>23&#45;&#45;34</title>
<path fill="none" stroke="black" d="M569.19,-121.49C552.64,-119.96 526.55,-117.55 508.85,-115.91"/>
</g>
<!-- 24 -->
<g id="node17" class="node">
<title>24</title>
<ellipse fill="none" stroke="black" cx="522.57" cy="-199.5" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="522.57" y="-197.6" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 26 -->
<g id="node19" class="node">
<title>26</title>
<ellipse fill="none" stroke="black" cx="485.91" cy="-276.78" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="485.91" y="-274.88" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 24&#45;&#45;26 -->
<g id="edge58" class="edge">
<title>24&#45;&#45;26</title>
<path fill="none" stroke="black" d="M517.16,-210.91C510.23,-225.52 498.28,-250.7 491.34,-265.33"/>
</g>
<!-- 24&#45;&#45;28 -->
<g id="edge59" class="edge">
<title>24&#45;&#45;28</title>
<path fill="none" stroke="black" d="M506.7,-199.73C496.39,-199.88 483.01,-200.07 472.73,-200.21"/>
</g>
<!-- 30 -->
<g id="node23" class="node">
<title>30</title>
<ellipse fill="none" stroke="black" cx="566.47" cy="-167.87" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="566.47" y="-165.97" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 24&#45;&#45;30 -->
<g id="edge60" class="edge">
<title>24&#45;&#45;30</title>
<path fill="none" stroke="black" d="M534.1,-191.19C540.51,-186.58 548.45,-180.86 554.86,-176.24"/>
</g>
<!-- 24&#45;&#45;33 -->
<g id="edge61" class="edge">
<title>24&#45;&#45;33</title>
<path fill="none" stroke="black" d="M520.52,-187.43C517.41,-169.1 511.52,-134.45 508.42,-116.18"/>
</g>
<!-- 24&#45;&#45;34 -->
<g id="edge62" class="edge">
<title>24&#45;&#45;34</title>
<path fill="none" stroke="black" d="M518.02,-187.64C511.77,-171.33 500.6,-142.18 494.37,-125.92"/>
</g>
<!-- 25 -->
<g id="node18" class="node">
<title>25</title>
<ellipse fill="none" stroke="black" cx="439.36" cy="-284.82" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="439.36" y="-282.92" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 25&#45;&#45;26 -->
<g id="edge63" class="edge">
<title>25&#45;&#45;26</title>
<path fill="none" stroke="black" d="M454.6,-282.19C459.69,-281.31 465.37,-280.33 470.47,-279.45"/>
</g>
<!-- 25&#45;&#45;28 -->
<g id="edge64" class="edge">
<title>25&#45;&#45;28</title>
<path fill="none" stroke="black" d="M441.88,-272.73C445.24,-256.69 451.13,-228.53 454.49,-212.5"/>
</g>
<!-- 25&#45;&#45;32 -->
<g id="edge65" class="edge">
<title>25&#45;&#45;32</title>
<path fill="none" stroke="black" d="M436,-272.77C430.91,-254.46 421.29,-219.86 416.22,-201.62"/>
</g>
<!-- 26&#45;&#45;32 -->
<g id="edge66" class="edge">
<title>26&#45;&#45;32</title>
<path fill="none" stroke="black" d="M477.25,-266.45C463.19,-249.68 435.59,-216.75 421.54,-199.99"/>
</g>
<!-- 27 -->
<g id="node20" class="node">
<title>27</title>
<ellipse fill="none" stroke="black" cx="614.32" cy="-172.85" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="614.32" y="-170.95" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 27&#45;&#45;30 -->
<g id="edge67" class="edge">
<title>27&#45;&#45;30</title>
<path fill="none" stroke="black" d="M598.65,-171.22C593.31,-170.66 587.34,-170.04 582.01,-169.49"/>
</g>
<!-- 27&#45;&#45;34 -->
<g id="edge68" class="edge">
<title>27&#45;&#45;34</title>
<path fill="none" stroke="black" d="M601,-166.57C577.8,-155.62 530.09,-133.13 505.41,-121.49"/>
</g>
<!-- 28&#45;&#45;34 -->
<g id="edge69" class="edge">
<title>28&#45;&#45;34</title>
<path fill="none" stroke="black" d="M461.44,-188.8C467.71,-172.33 479.11,-142.4 485.4,-125.89"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge70" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M430.85,-163.76C427.56,-168.5 423.71,-174.05 420.43,-178.8"/>
</g>
<!-- 29&#45;&#45;34 -->
<g id="edge71" class="edge">
<title>29&#45;&#45;34</title>
<path fill="none" stroke="black" d="M449.52,-144.53C457.69,-138.38 468.77,-130.03 477.26,-123.65"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge72" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M557.08,-157.92C545.86,-146.03 527.19,-126.25 515.9,-114.29"/>
</g>
<!-- 30&#45;&#45;34 -->
<g id="edge73" class="edge">
<title>30&#45;&#45;34</title>
<path fill="none" stroke="black" d="M554.84,-159.71C540.77,-149.85 517.18,-133.31 502.63,-123.11"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge74" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M439.07,-66.72C454.03,-75.05 478.58,-88.72 493.53,-97.05"/>
</g>
<!-- 31&#45;&#45;34 -->
<g id="edge75" class="edge">
<title>31&#45;&#45;34</title>
<path fill="none" stroke="black" d="M436.7,-68.56C448.14,-78.38 466.39,-94.03 478.25,-104.2"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge76" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M423.25,-180.2C441.32,-163.68 478.36,-129.82 496.26,-113.46"/>
</g>
<!-- 32&#45;&#45;34 -->
<g id="edge77" class="edge">
<title>32&#45;&#45;34</title>
<path fill="none" stroke="black" d="M422.64,-180.1C437.2,-165.82 464.52,-139.02 479.46,-124.37"/>
</g>
<!-- 33&#45;&#45;34 -->
<g id="edge78" class="edge">
<title>33&#45;&#45;34</title>
<path fill="none" stroke="black" d="M494.03,-111.65C493.88,-111.74 493.73,-111.83 493.59,-111.91"/>
</g>
</g>
</svg>
</div>

## Triangle Centrality

Based on the Algorithm from https://arxiv.org/abs/2105.00110

Triangle Centrality is a node ranking metric for finding nodes in
the center of the most triangles.  Triangles represent strongly
connected nodes and their centrality can convey more importance
than node centrality.

``` postgres-console
CREATE OR REPLACE FUNCTION tcentrality(a matrix)
    RETURNS vector LANGUAGE plpgsql AS
    $$
    DECLARE
        n bigint;  -- number of rows in a
        l matrix;  -- lower triangle of a
        t matrix;  -- triangle count
        c vector;
        y vector;
        w vector;
        u vector;
        k scalar;
        plus_monoid monoid = 'plus_monoid_fp32';
        plus_pair semiring = 'plus_pair_fp32';
        plus_second semiring = 'plus_second_fp32';
        times binaryop = 'times_fp32';
        div binaryop = 'div_fp32';
        plus binaryop = 'plus_fp32';
        st1 descriptor = 'st1';
        t0 descriptor = 't0';
        rtype type = 'fp32';
    BEGIN
        a = wait(a);
        n = nrows(a);
        y = vector(rtype, n);
        w = vector(rtype, n);
        u = vector(rtype, n);
        c = vector(rtype, n);
        l = tril(a, -1);
        t = matrix(rtype, n, n);
        t = mxm(a, a, plus_pair, mask=>l, c=>t, descr=>st1);
        y = assign(y, 0);
        y = reduce_cols(t, plus_monoid, accum=>plus, descr=>t0);
        y = reduce_cols(t, plus_monoid, accum=>plus);
        k = reduce_scalar(y);
        --        RETURN 3 * ((a @ y) |- 2 * (one(t) @ y) |+ y) / k;
        w = mxv(t, y, plus_second, c=>w);
        w = mxv(t, y, plus_second, accum=>plus, c=>w, descr=>t0);
        w = apply(w, -2, times, c=>w);
        u = mxv(a, y, plus_second, c=>u);
        c = apply(3, u, times, c=>c);
        c = eadd(w, y, plus, accum=>plus, c=>c);
        c = apply(c, k, div, c=>c);
        return c;
    END;
    $$;
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="963pt" height="443pt"
 viewBox="0.00 0.00 963.33 442.98" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 438.98)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-438.98 959.33,-438.98 959.33,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="369.78" cy="-177.86" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="369.78" y="-175.96" font-family="Times,serif" font-size="8.00">0.922222</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="454.61" cy="-89.64" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="454.61" y="-87.74" font-family="Times,serif" font-size="8.00">0.688889</text>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge1" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M381.22,-165.96C397.5,-149.03 427.17,-118.18 443.35,-101.35"/>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="554.48" cy="-150.6" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="554.48" y="-148.7" font-family="Times,serif" font-size="8.00">0.688889</text>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge2" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M405.39,-172.6C437.99,-167.79 486,-160.7 518.66,-155.88"/>
</g>
<!-- 4 -->
<g id="node4" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="404.24" cy="-82.74" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="404.24" y="-80.84" font-family="Times,serif" font-size="8.00">0.611111</text>
</g>
<!-- 1&#45;&#45;4 -->
<g id="edge3" class="edge">
<title>1&#45;&#45;4</title>
<path fill="none" stroke="black" d="M374.14,-165.81C380.73,-147.61 393.16,-113.3 399.8,-94.99"/>
</g>
<!-- 5 -->
<g id="node5" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="210.2" cy="-188.14" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="210.2" y="-186.24" font-family="Times,serif" font-size="8.00">0.122222</text>
</g>
<!-- 1&#45;&#45;5 -->
<g id="edge4" class="edge">
<title>1&#45;&#45;5</title>
<path fill="none" stroke="black" d="M330.74,-180.37C305.92,-181.97 273.91,-184.03 249.11,-185.63"/>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="146.37" cy="-195.75" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="146.37" y="-193.85" font-family="Times,serif" font-size="8.00">0.144444</text>
</g>
<!-- 1&#45;&#45;6 -->
<g id="edge5" class="edge">
<title>1&#45;&#45;6</title>
<path fill="none" stroke="black" d="M331.37,-180.93C290.45,-184.21 226.03,-189.37 185.02,-192.65"/>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="152.25" cy="-240.52" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="152.25" y="-238.62" font-family="Times,serif" font-size="8.00">0.122222</text>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge6" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M340.89,-186.18C299.28,-198.16 222.93,-220.16 181.26,-232.17"/>
</g>
<!-- 9 -->
<g id="node8" class="node">
<title>9</title>
<ellipse fill="none" stroke="black" cx="597.72" cy="-148.92" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="597.72" y="-147.02" font-family="Times,serif" font-size="8.00">0.211111</text>
</g>
<!-- 1&#45;&#45;9 -->
<g id="edge8" class="edge">
<title>1&#45;&#45;9</title>
<path fill="none" stroke="black" d="M406.4,-173.21C448.81,-167.82 518.28,-159 560.81,-153.6"/>
</g>
<!-- 14 -->
<g id="node10" class="node">
<title>14</title>
<ellipse fill="none" stroke="black" cx="518.97" cy="-105.45" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="518.97" y="-103.55" font-family="Times,serif" font-size="8.00">0.455556</text>
</g>
<!-- 1&#45;&#45;14 -->
<g id="edge12" class="edge">
<title>1&#45;&#45;14</title>
<path fill="none" stroke="black" d="M390.83,-167.64C419.11,-153.91 469.07,-129.67 497.53,-115.85"/>
</g>
<!-- 20 -->
<g id="node14" class="node">
<title>20</title>
<ellipse fill="none" stroke="black" cx="518.72" cy="-194.62" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="518.72" y="-192.72" font-family="Times,serif" font-size="8.00">0.100000</text>
</g>
<!-- 1&#45;&#45;20 -->
<g id="edge14" class="edge">
<title>1&#45;&#45;20</title>
<path fill="none" stroke="black" d="M406.98,-182.04C429.72,-184.6 458.72,-187.86 481.48,-190.42"/>
</g>
<!-- 32 -->
<g id="node25" class="node">
<title>32</title>
<ellipse fill="none" stroke="black" cx="612.31" cy="-279.63" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="612.31" y="-277.73" font-family="Times,serif" font-size="8.00">0.233333</text>
</g>
<!-- 1&#45;&#45;32 -->
<g id="edge16" class="edge">
<title>1&#45;&#45;32</title>
<path fill="none" stroke="black" d="M393.03,-187.61C439.27,-207.02 542.3,-250.25 588.77,-269.75"/>
</g>
<!-- 8 -->
<g id="node27" class="node">
<title>8</title>
<ellipse fill="none" stroke="black" cx="424.67" cy="-152.41" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="424.67" y="-150.51" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&#45;8 -->
<g id="edge7" class="edge">
<title>1&#45;&#45;8</title>
<path fill="none" stroke="black" d="M391.47,-167.8C398.07,-164.74 405.12,-161.47 411.03,-158.74"/>
</g>
<!-- 11 -->
<g id="node28" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="220.07" cy="-250.37" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="220.07" y="-248.47" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 1&#45;&#45;11 -->
<g id="edge9" class="edge">
<title>1&#45;&#45;11</title>
<path fill="none" stroke="black" d="M348.64,-188.09C318.18,-202.85 262.62,-229.76 235.62,-242.84"/>
</g>
<!-- 12 -->
<g id="node29" class="node">
<title>12</title>
<ellipse fill="none" stroke="black" cx="285.67" cy="-337.71" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="285.67" y="-335.81" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1&#45;&#45;12 -->
<g id="edge10" class="edge">
<title>1&#45;&#45;12</title>
<path fill="none" stroke="black" d="M363.45,-189.87C347.79,-219.64 307.34,-296.53 291.84,-325.99"/>
</g>
<!-- 13 -->
<g id="node30" class="node">
<title>13</title>
<ellipse fill="none" stroke="black" cx="270.95" cy="-45.39" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="270.95" y="-43.49" font-family="Times,serif" font-size="8.00">13</text>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge11" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M361.01,-166.1C342.11,-140.77 297.77,-81.35 279.28,-56.56"/>
</g>
<!-- 18 -->
<g id="node31" class="node">
<title>18</title>
<ellipse fill="none" stroke="black" cx="313.07" cy="-86.89" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="313.07" y="-84.99" font-family="Times,serif" font-size="8.00">18</text>
</g>
<!-- 1&#45;&#45;18 -->
<g id="edge13" class="edge">
<title>1&#45;&#45;18</title>
<path fill="none" stroke="black" d="M362.36,-165.96C351.37,-148.34 330.91,-115.5 320.13,-98.21"/>
</g>
<!-- 22 -->
<g id="node32" class="node">
<title>22</title>
<ellipse fill="none" stroke="black" cx="349.93" cy="-12.02" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="349.93" y="-10.12" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&#45;22 -->
<g id="edge15" class="edge">
<title>1&#45;&#45;22</title>
<path fill="none" stroke="black" d="M368.32,-165.66C364.64,-134.99 355.06,-54.92 351.39,-24.23"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge17" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M472.46,-100.54C490.64,-111.63 518.66,-128.73 536.78,-139.79"/>
</g>
<!-- 2&#45;&#45;4 -->
<g id="edge18" class="edge">
<title>2&#45;&#45;4</title>
<path fill="none" stroke="black" d="M418.27,-84.66C418.14,-84.64 418.02,-84.63 417.9,-84.61"/>
</g>
<!-- 2&#45;&#45;14 -->
<g id="edge20" class="edge">
<title>2&#45;&#45;14</title>
<path fill="none" stroke="black" d="M485.66,-97.27C486.43,-97.46 487.2,-97.65 487.97,-97.84"/>
</g>
<!-- 2&#45;&#45;20 -->
<g id="edge22" class="edge">
<title>2&#45;&#45;20</title>
<path fill="none" stroke="black" d="M461.96,-101.68C474.33,-121.93 499.14,-162.56 511.45,-182.71"/>
</g>
<!-- 31 -->
<g id="node24" class="node">
<title>31</title>
<ellipse fill="none" stroke="black" cx="632.33" cy="-83.6" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="632.33" y="-81.7" font-family="Times,serif" font-size="8.00">0.344444</text>
</g>
<!-- 2&#45;&#45;31 -->
<g id="edge24" class="edge">
<title>2&#45;&#45;31</title>
<path fill="none" stroke="black" d="M494.05,-88.3C523.4,-87.3 563.29,-85.95 592.69,-84.95"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge19" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M448.84,-101.74C443.47,-113 435.52,-129.66 430.23,-140.77"/>
</g>
<!-- 2&#45;&#45;18 -->
<g id="edge21" class="edge">
<title>2&#45;&#45;18</title>
<path fill="none" stroke="black" d="M414.8,-88.87C387.82,-88.34 353.36,-87.67 332.27,-87.26"/>
</g>
<!-- 2&#45;&#45;22 -->
<g id="edge23" class="edge">
<title>2&#45;&#45;22</title>
<path fill="none" stroke="black" d="M439.16,-78.19C418.35,-62.75 381.69,-35.57 362.48,-21.33"/>
</g>
<!-- 3&#45;&#45;4 -->
<g id="edge25" class="edge">
<title>3&#45;&#45;4</title>
<path fill="none" stroke="black" d="M532.31,-140.58C503.91,-127.75 454.95,-105.64 426.5,-92.79"/>
</g>
<!-- 3&#45;&#45;9 -->
<g id="edge27" class="edge">
<title>3&#45;&#45;9</title>
<path fill="none" stroke="black" d="M594.03,-149.06C594.17,-149.06 594.31,-149.05 594.45,-149.04"/>
</g>
<!-- 10 -->
<g id="node9" class="node">
<title>10</title>
<ellipse fill="none" stroke="black" cx="667.89" cy="-32.34" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="667.89" y="-30.44" font-family="Times,serif" font-size="8.00">0.300000</text>
</g>
<!-- 3&#45;&#45;10 -->
<g id="edge28" class="edge">
<title>3&#45;&#45;10</title>
<path fill="none" stroke="black" d="M565.77,-138.82C587.51,-116.16 634.94,-66.69 656.65,-44.06"/>
</g>
<!-- 3&#45;&#45;14 -->
<g id="edge29" class="edge">
<title>3&#45;&#45;14</title>
<path fill="none" stroke="black" d="M545.15,-138.73C539.97,-132.15 533.55,-123.99 528.36,-117.39"/>
</g>
<!-- 28 -->
<g id="node21" class="node">
<title>28</title>
<ellipse fill="none" stroke="black" cx="678.77" cy="-295.85" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="678.77" y="-293.95" font-family="Times,serif" font-size="8.00">0.300000</text>
</g>
<!-- 3&#45;&#45;28 -->
<g id="edge30" class="edge">
<title>3&#45;&#45;28</title>
<path fill="none" stroke="black" d="M564.65,-162.48C588.07,-189.86 645.59,-257.07 668.81,-284.2"/>
</g>
<!-- 29 -->
<g id="node22" class="node">
<title>29</title>
<ellipse fill="none" stroke="black" cx="650.65" cy="-224.3" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="650.65" y="-222.4" font-family="Times,serif" font-size="8.00">0.366667</text>
</g>
<!-- 3&#45;&#45;29 -->
<g id="edge31" class="edge">
<title>3&#45;&#45;29</title>
<path fill="none" stroke="black" d="M569.09,-161.79C587.16,-175.64 617.77,-199.1 635.91,-213"/>
</g>
<!-- 33 -->
<g id="node26" class="node">
<title>33</title>
<ellipse fill="none" stroke="black" cx="753.15" cy="-150.89" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="753.15" y="-148.99" font-family="Times,serif" font-size="8.00">0.288889</text>
</g>
<!-- 3&#45;&#45;33 -->
<g id="edge32" class="edge">
<title>3&#45;&#45;33</title>
<path fill="none" stroke="black" d="M594.21,-150.65C628.86,-150.71 678.86,-150.78 713.49,-150.83"/>
</g>
<!-- 3&#45;&#45;8 -->
<g id="edge26" class="edge">
<title>3&#45;&#45;8</title>
<path fill="none" stroke="black" d="M514.83,-151.15C489.78,-151.5 458.78,-151.93 440.43,-152.19"/>
</g>
<!-- 4&#45;&#45;14 -->
<g id="edge35" class="edge">
<title>4&#45;&#45;14</title>
<path fill="none" stroke="black" d="M437.42,-89.31C452.51,-92.3 470.33,-95.82 485.46,-98.82"/>
</g>
<!-- 4&#45;&#45;8 -->
<g id="edge33" class="edge">
<title>4&#45;&#45;8</title>
<path fill="none" stroke="black" d="M407.8,-94.88C411.55,-107.66 417.42,-127.68 421.16,-140.41"/>
</g>
<!-- 4&#45;&#45;13 -->
<g id="edge34" class="edge">
<title>4&#45;&#45;13</title>
<path fill="none" stroke="black" d="M374.99,-74.54C348.63,-67.16 310.72,-56.54 288.69,-50.36"/>
</g>
<!-- 5&#45;&#45;7 -->
<g id="edge36" class="edge">
<title>5&#45;&#45;7</title>
<path fill="none" stroke="black" d="M197.34,-199.76C187.74,-208.44 174.67,-220.26 165.08,-228.93"/>
</g>
<!-- 5&#45;&#45;11 -->
<g id="edge37" class="edge">
<title>5&#45;&#45;11</title>
<path fill="none" stroke="black" d="M212.15,-200.43C213.88,-211.34 216.4,-227.2 218.13,-238.1"/>
</g>
<!-- 6&#45;&#45;7 -->
<g id="edge38" class="edge">
<title>6&#45;&#45;7</title>
<path fill="none" stroke="black" d="M147.97,-207.99C148.8,-214.3 149.81,-221.99 150.64,-228.3"/>
</g>
<!-- 6&#45;&#45;11 -->
<g id="edge39" class="edge">
<title>6&#45;&#45;11</title>
<path fill="none" stroke="black" d="M161.64,-207.07C175.13,-217.07 194.58,-231.48 207.28,-240.89"/>
</g>
<!-- 17 -->
<g id="node33" class="node">
<title>17</title>
<ellipse fill="none" stroke="black" cx="19.09" cy="-239.97" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="19.09" y="-238.07" font-family="Times,serif" font-size="8.00">17</text>
</g>
<!-- 6&#45;&#45;17 -->
<g id="edge40" class="edge">
<title>6&#45;&#45;17</title>
<path fill="none" stroke="black" d="M120.3,-204.81C95.14,-213.55 57.77,-226.53 36.18,-234.04"/>
</g>
<!-- 7&#45;&#45;17 -->
<g id="edge41" class="edge">
<title>7&#45;&#45;17</title>
<path fill="none" stroke="black" d="M112.3,-240.36C87.78,-240.26 57.5,-240.13 38.24,-240.05"/>
</g>
<!-- 9&#45;&#45;31 -->
<g id="edge42" class="edge">
<title>9&#45;&#45;31</title>
<path fill="none" stroke="black" d="M604.07,-136.94C610.31,-125.15 619.79,-107.27 626.02,-95.51"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge43" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M637.34,-149.42C660.64,-149.72 690,-150.09 713.33,-150.38"/>
</g>
<!-- 34 -->
<g id="node34" class="node">
<title>34</title>
<ellipse fill="none" stroke="black" cx="728.26" cy="-165.88" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="728.26" y="-163.98" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 9&#45;&#45;34 -->
<g id="edge44" class="edge">
<title>9&#45;&#45;34</title>
<path fill="none" stroke="black" d="M634.43,-153.69C658.86,-156.86 689.95,-160.9 709.48,-163.44"/>
</g>
<!-- 10&#45;&#45;34 -->
<g id="edge45" class="edge">
<title>10&#45;&#45;34</title>
<path fill="none" stroke="black" d="M673.46,-44.66C685.02,-70.23 711.48,-128.76 722.87,-153.97"/>
</g>
<!-- 14&#45;&#45;34 -->
<g id="edge46" class="edge">
<title>14&#45;&#45;34</title>
<path fill="none" stroke="black" d="M548.07,-113.85C592.21,-126.6 674.75,-150.43 710.73,-160.82"/>
</g>
<!-- 15 -->
<g id="node11" class="node">
<title>15</title>
<ellipse fill="none" stroke="black" cx="788.84" cy="-29.27" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="788.84" y="-27.37" font-family="Times,serif" font-size="8.00">0.200000</text>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge47" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M785.29,-41.38C778.45,-64.69 763.52,-115.56 756.68,-138.83"/>
</g>
<!-- 15&#45;&#45;34 -->
<g id="edge48" class="edge">
<title>15&#45;&#45;34</title>
<path fill="none" stroke="black" d="M783.46,-41.4C771.94,-67.38 744.97,-128.19 733.54,-153.98"/>
</g>
<!-- 16 -->
<g id="node12" class="node">
<title>16</title>
<ellipse fill="none" stroke="black" cx="893.62" cy="-128.77" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="893.62" y="-126.87" font-family="Times,serif" font-size="8.00">0.200000</text>
</g>
<!-- 16&#45;&#45;33 -->
<g id="edge49" class="edge">
<title>16&#45;&#45;33</title>
<path fill="none" stroke="black" d="M858.17,-134.35C836.88,-137.71 809.87,-141.96 788.58,-145.31"/>
</g>
<!-- 16&#45;&#45;34 -->
<g id="edge50" class="edge">
<title>16&#45;&#45;34</title>
<path fill="none" stroke="black" d="M861.73,-135.93C827.38,-143.64 773.94,-155.63 746.25,-161.84"/>
</g>
<!-- 19 -->
<g id="node13" class="node">
<title>19</title>
<ellipse fill="none" stroke="black" cx="842.86" cy="-45.31" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="842.86" y="-43.41" font-family="Times,serif" font-size="8.00">0.200000</text>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge51" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M832.91,-57.01C815.67,-77.31 780.41,-118.8 763.13,-139.13"/>
</g>
<!-- 19&#45;&#45;34 -->
<g id="edge52" class="edge">
<title>19&#45;&#45;34</title>
<path fill="none" stroke="black" d="M831.87,-56.87C809.57,-80.33 759.33,-133.19 738.06,-155.56"/>
</g>
<!-- 20&#45;&#45;34 -->
<g id="edge53" class="edge">
<title>20&#45;&#45;34</title>
<path fill="none" stroke="black" d="M555.22,-189.61C600.13,-183.45 674.68,-173.23 709.39,-168.47"/>
</g>
<!-- 21 -->
<g id="node15" class="node">
<title>21</title>
<ellipse fill="none" stroke="black" cx="856.16" cy="-94.83" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="856.16" y="-92.93" font-family="Times,serif" font-size="8.00">0.200000</text>
</g>
<!-- 21&#45;&#45;33 -->
<g id="edge54" class="edge">
<title>21&#45;&#45;33</title>
<path fill="none" stroke="black" d="M836.78,-105.38C818.48,-115.34 791.17,-130.2 772.79,-140.2"/>
</g>
<!-- 21&#45;&#45;34 -->
<g id="edge55" class="edge">
<title>21&#45;&#45;34</title>
<path fill="none" stroke="black" d="M837.01,-105.47C811.23,-119.79 765.91,-144.96 742.76,-157.82"/>
</g>
<!-- 23 -->
<g id="node16" class="node">
<title>23</title>
<ellipse fill="none" stroke="black" cx="871.39" cy="-179.11" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="871.39" y="-177.21" font-family="Times,serif" font-size="8.00">0.200000</text>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge56" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M840.02,-171.62C822.91,-167.54 801.8,-162.5 784.67,-158.41"/>
</g>
<!-- 23&#45;&#45;34 -->
<g id="edge57" class="edge">
<title>23&#45;&#45;34</title>
<path fill="none" stroke="black" d="M833.41,-175.6C805.75,-173.04 769.43,-169.69 747.52,-167.66"/>
</g>
<!-- 24 -->
<g id="node17" class="node">
<title>24</title>
<ellipse fill="none" stroke="black" cx="777.53" cy="-294.45" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="777.53" y="-292.55" font-family="Times,serif" font-size="8.00">0.111111</text>
</g>
<!-- 26 -->
<g id="node19" class="node">
<title>26</title>
<ellipse fill="none" stroke="black" cx="722.31" cy="-410.86" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="722.31" y="-408.96" font-family="Times,serif" font-size="8.00">0.033333</text>
</g>
<!-- 24&#45;&#45;26 -->
<g id="edge58" class="edge">
<title>24&#45;&#45;26</title>
<path fill="none" stroke="black" d="M771.83,-306.47C761.24,-328.77 738.68,-376.34 728.06,-398.73"/>
</g>
<!-- 24&#45;&#45;28 -->
<g id="edge59" class="edge">
<title>24&#45;&#45;28</title>
<path fill="none" stroke="black" d="M737.93,-295.01C731.49,-295.1 724.82,-295.19 718.38,-295.28"/>
</g>
<!-- 30 -->
<g id="node23" class="node">
<title>30</title>
<ellipse fill="none" stroke="black" cx="843.66" cy="-246.79" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="843.66" y="-244.89" font-family="Times,serif" font-size="8.00">0.077778</text>
</g>
<!-- 24&#45;&#45;30 -->
<g id="edge60" class="edge">
<title>24&#45;&#45;30</title>
<path fill="none" stroke="black" d="M793.2,-283.15C803.74,-275.56 817.58,-265.58 828.1,-258"/>
</g>
<!-- 24&#45;&#45;33 -->
<g id="edge61" class="edge">
<title>24&#45;&#45;33</title>
<path fill="none" stroke="black" d="M775.45,-282.21C770.84,-255.08 759.82,-190.19 755.22,-163.1"/>
</g>
<!-- 24&#45;&#45;34 -->
<g id="edge62" class="edge">
<title>24&#45;&#45;34</title>
<path fill="none" stroke="black" d="M772.8,-282.12C763.31,-257.36 742.05,-201.87 732.75,-177.61"/>
</g>
<!-- 25 -->
<g id="node18" class="node">
<title>25</title>
<ellipse fill="none" stroke="black" cx="652.18" cy="-422.96" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="652.18" y="-421.06" font-family="Times,serif" font-size="8.00">0.033333</text>
</g>
<!-- 25&#45;&#45;26 -->
<g id="edge63" class="edge">
<title>25&#45;&#45;26</title>
<path fill="none" stroke="black" d="M686.83,-416.98C687.11,-416.93 687.38,-416.88 687.66,-416.84"/>
</g>
<!-- 25&#45;&#45;28 -->
<g id="edge64" class="edge">
<title>25&#45;&#45;28</title>
<path fill="none" stroke="black" d="M654.73,-410.77C659.8,-386.54 671.09,-332.54 676.19,-308.18"/>
</g>
<!-- 25&#45;&#45;32 -->
<g id="edge65" class="edge">
<title>25&#45;&#45;32</title>
<path fill="none" stroke="black" d="M648.78,-410.74C641.24,-383.66 623.22,-318.87 615.7,-291.82"/>
</g>
<!-- 26&#45;&#45;32 -->
<g id="edge66" class="edge">
<title>26&#45;&#45;32</title>
<path fill="none" stroke="black" d="M712.15,-398.75C691.09,-373.62 642.88,-316.1 622.12,-291.33"/>
</g>
<!-- 27 -->
<g id="node20" class="node">
<title>27</title>
<ellipse fill="none" stroke="black" cx="915.73" cy="-254.29" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="915.73" y="-252.39" font-family="Times,serif" font-size="8.00">0.033333</text>
</g>
<!-- 27&#45;&#45;30 -->
<g id="edge67" class="edge">
<title>27&#45;&#45;30</title>
<path fill="none" stroke="black" d="M878.01,-250.36C877.87,-250.35 877.74,-250.34 877.6,-250.32"/>
</g>
<!-- 27&#45;&#45;34 -->
<g id="edge68" class="edge">
<title>27&#45;&#45;34</title>
<path fill="none" stroke="black" d="M894.23,-244.15C855.95,-226.1 776.89,-188.81 743.59,-173.11"/>
</g>
<!-- 28&#45;&#45;34 -->
<g id="edge69" class="edge">
<title>28&#45;&#45;34</title>
<path fill="none" stroke="black" d="M683.34,-283.85C692.77,-259.09 714.28,-202.58 723.7,-177.85"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge70" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M642.33,-236.31C635.9,-245.59 627.04,-258.37 620.61,-267.65"/>
</g>
<!-- 29&#45;&#45;34 -->
<g id="edge71" class="edge">
<title>29&#45;&#45;34</title>
<path fill="none" stroke="black" d="M665.62,-213.04C680.24,-202.03 702.24,-185.47 715.9,-175.19"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge72" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M832.56,-235.03C815.1,-216.52 781.4,-180.82 764.05,-162.45"/>
</g>
<!-- 30&#45;&#45;34 -->
<g id="edge73" class="edge">
<title>30&#45;&#45;34</title>
<path fill="none" stroke="black" d="M827.62,-235.54C804.58,-219.39 762.57,-189.93 741.31,-175.03"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge74" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M651.47,-94.27C674.09,-106.86 711.51,-127.7 734.09,-140.28"/>
</g>
<!-- 31&#45;&#45;34 -->
<g id="edge75" class="edge">
<title>31&#45;&#45;34</title>
<path fill="none" stroke="black" d="M645.67,-95.04C664.61,-111.29 698.99,-140.78 716.83,-156.08"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge76" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M624.81,-268.2C651.47,-243.83 713.66,-186.98 740.47,-162.47"/>
</g>
<!-- 32&#45;&#45;34 -->
<g id="edge77" class="edge">
<title>32&#45;&#45;34</title>
<path fill="none" stroke="black" d="M624.28,-267.88C647.08,-245.52 696.36,-197.17 717.87,-176.07"/>
</g>
<!-- 33&#45;&#45;34 -->
<g id="edge78" class="edge">
<title>33&#45;&#45;34</title>
<path fill="none" stroke="black" d="M735.32,-161.62C735.2,-161.7 735.08,-161.77 734.95,-161.85"/>
</g>
</g>
</svg>
</div>

## Page Rank TODO
``` postgres-console
create or replace function pagerank(
    a matrix,
    damping float default 0.85,
    itermax integer default 100,
    tol float default 1e-4
    )
    returns vector language plpgsql as
    $$
    declare
        rtype type = 'fp32';
        n bigint;
        teleport float;
        rdiff float = 1.0;
        d_out vector;
        t vector;
        w vector;
        r vector;
        plus_second semiring = 'plus_second_fp32';
        abs unaryop = 'abs_fp32';
        plus binaryop = 'plus_fp32';
        div binaryop = 'div_fp32';
        t0 descriptor = 't0';
    begin
        a = wait(a);
        n = nrows(a);
        teleport = (1 - damping) / n;
        d_out = cast_to(reduce_rows(one(a)), rtype);
        t = vector(rtype, n);
        w = vector(rtype, n);
        r = vector(rtype, n);
        r = assign(r, 1::float / n);
        d_out = assign(d_out, damping::scalar, accum=>div);
        for i in 0..itermax loop
            -- select r, t into t, r;
            t = dup(r);
            r = assign(r, teleport);
            w = eadd(t, d_out, div, c=>w);
            r = mxv(a, w, plus_second, c=>r, accum=>plus, descr=>t0);
            t = apply(t, abs, c=>t);
            rdiff = reduce_scalar(t);
            exit when rdiff < tol;
        end loop;
        return r;
    end;
    $$;
```
<div>
<!-- Title: %3 Pages: 1 -->
<svg width="963pt" height="443pt"
 viewBox="0.00 0.00 963.33 442.98" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 438.98)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-438.98 959.33,-438.98 959.33,4 -4,4"/>
<!-- 1 -->
<g id="node1" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="369.78" cy="-177.86" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="369.78" y="-175.96" font-family="Times,serif" font-size="8.00">0.109104</text>
</g>
<!-- 2 -->
<g id="node2" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="454.61" cy="-89.64" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="454.61" y="-87.74" font-family="Times,serif" font-size="8.00">0.060958</text>
</g>
<!-- 1&#45;&#45;2 -->
<g id="edge1" class="edge">
<title>1&#45;&#45;2</title>
<path fill="none" stroke="black" d="M381.22,-165.96C397.5,-149.03 427.17,-118.18 443.35,-101.35"/>
</g>
<!-- 3 -->
<g id="node3" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="554.48" cy="-150.6" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="554.48" y="-148.7" font-family="Times,serif" font-size="8.00">0.070775</text>
</g>
<!-- 1&#45;&#45;3 -->
<g id="edge2" class="edge">
<title>1&#45;&#45;3</title>
<path fill="none" stroke="black" d="M405.39,-172.6C437.99,-167.79 486,-160.7 518.66,-155.88"/>
</g>
<!-- 4 -->
<g id="node4" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="404.24" cy="-82.74" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="404.24" y="-80.84" font-family="Times,serif" font-size="8.00">0.040543</text>
</g>
<!-- 1&#45;&#45;4 -->
<g id="edge3" class="edge">
<title>1&#45;&#45;4</title>
<path fill="none" stroke="black" d="M374.14,-165.81C380.73,-147.61 393.16,-113.3 399.8,-94.99"/>
</g>
<!-- 5 -->
<g id="node5" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="210.2" cy="-188.14" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="210.2" y="-186.24" font-family="Times,serif" font-size="8.00">0.023384</text>
</g>
<!-- 1&#45;&#45;5 -->
<g id="edge4" class="edge">
<title>1&#45;&#45;5</title>
<path fill="none" stroke="black" d="M330.74,-180.37C305.92,-181.97 273.91,-184.03 249.11,-185.63"/>
</g>
<!-- 6 -->
<g id="node6" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="146.37" cy="-195.75" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="146.37" y="-193.85" font-family="Times,serif" font-size="8.00">0.030828</text>
</g>
<!-- 1&#45;&#45;6 -->
<g id="edge5" class="edge">
<title>1&#45;&#45;6</title>
<path fill="none" stroke="black" d="M331.37,-180.93C290.45,-184.21 226.03,-189.37 185.02,-192.65"/>
</g>
<!-- 7 -->
<g id="node7" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="152.25" cy="-240.52" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="152.25" y="-238.62" font-family="Times,serif" font-size="8.00">0.030828</text>
</g>
<!-- 1&#45;&#45;7 -->
<g id="edge6" class="edge">
<title>1&#45;&#45;7</title>
<path fill="none" stroke="black" d="M340.89,-186.18C299.28,-198.16 222.93,-220.16 181.26,-232.17"/>
</g>
<!-- 9 -->
<g id="node8" class="node">
<title>9</title>
<ellipse fill="none" stroke="black" cx="597.72" cy="-148.92" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="597.72" y="-147.02" font-family="Times,serif" font-size="8.00">0.030591</text>
</g>
<!-- 1&#45;&#45;9 -->
<g id="edge8" class="edge">
<title>1&#45;&#45;9</title>
<path fill="none" stroke="black" d="M406.4,-173.21C448.81,-167.82 518.28,-159 560.81,-153.6"/>
</g>
<!-- 14 -->
<g id="node10" class="node">
<title>14</title>
<ellipse fill="none" stroke="black" cx="518.97" cy="-105.45" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="518.97" y="-103.55" font-family="Times,serif" font-size="8.00">0.027725</text>
</g>
<!-- 1&#45;&#45;14 -->
<g id="edge12" class="edge">
<title>1&#45;&#45;14</title>
<path fill="none" stroke="black" d="M390.83,-167.64C419.11,-153.91 469.07,-129.67 497.53,-115.85"/>
</g>
<!-- 20 -->
<g id="node14" class="node">
<title>20</title>
<ellipse fill="none" stroke="black" cx="518.72" cy="-194.62" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="518.72" y="-192.72" font-family="Times,serif" font-size="8.00">0.015965</text>
</g>
<!-- 1&#45;&#45;20 -->
<g id="edge14" class="edge">
<title>1&#45;&#45;20</title>
<path fill="none" stroke="black" d="M406.98,-182.04C429.72,-184.6 458.72,-187.86 481.48,-190.42"/>
</g>
<!-- 32 -->
<g id="node25" class="node">
<title>32</title>
<ellipse fill="none" stroke="black" cx="612.31" cy="-279.63" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="612.31" y="-277.73" font-family="Times,serif" font-size="8.00">0.039572</text>
</g>
<!-- 1&#45;&#45;32 -->
<g id="edge16" class="edge">
<title>1&#45;&#45;32</title>
<path fill="none" stroke="black" d="M393.03,-187.61C439.27,-207.02 542.3,-250.25 588.77,-269.75"/>
</g>
<!-- 8 -->
<g id="node27" class="node">
<title>8</title>
<ellipse fill="none" stroke="black" cx="424.67" cy="-152.41" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="424.67" y="-150.51" font-family="Times,serif" font-size="8.00">8</text>
</g>
<!-- 1&#45;&#45;8 -->
<g id="edge7" class="edge">
<title>1&#45;&#45;8</title>
<path fill="none" stroke="black" d="M391.47,-167.8C398.07,-164.74 405.12,-161.47 411.03,-158.74"/>
</g>
<!-- 11 -->
<g id="node28" class="node">
<title>11</title>
<ellipse fill="none" stroke="black" cx="220.07" cy="-250.37" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="220.07" y="-248.47" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 1&#45;&#45;11 -->
<g id="edge9" class="edge">
<title>1&#45;&#45;11</title>
<path fill="none" stroke="black" d="M348.64,-188.09C318.18,-202.85 262.62,-229.76 235.62,-242.84"/>
</g>
<!-- 12 -->
<g id="node29" class="node">
<title>12</title>
<ellipse fill="none" stroke="black" cx="285.67" cy="-337.71" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="285.67" y="-335.81" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 1&#45;&#45;12 -->
<g id="edge10" class="edge">
<title>1&#45;&#45;12</title>
<path fill="none" stroke="black" d="M363.45,-189.87C347.79,-219.64 307.34,-296.53 291.84,-325.99"/>
</g>
<!-- 13 -->
<g id="node30" class="node">
<title>13</title>
<ellipse fill="none" stroke="black" cx="270.95" cy="-45.39" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="270.95" y="-43.49" font-family="Times,serif" font-size="8.00">13</text>
</g>
<!-- 1&#45;&#45;13 -->
<g id="edge11" class="edge">
<title>1&#45;&#45;13</title>
<path fill="none" stroke="black" d="M361.01,-166.1C342.11,-140.77 297.77,-81.35 279.28,-56.56"/>
</g>
<!-- 18 -->
<g id="node31" class="node">
<title>18</title>
<ellipse fill="none" stroke="black" cx="313.07" cy="-86.89" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="313.07" y="-84.99" font-family="Times,serif" font-size="8.00">18</text>
</g>
<!-- 1&#45;&#45;18 -->
<g id="edge13" class="edge">
<title>1&#45;&#45;18</title>
<path fill="none" stroke="black" d="M362.36,-165.96C351.37,-148.34 330.91,-115.5 320.13,-98.21"/>
</g>
<!-- 22 -->
<g id="node32" class="node">
<title>22</title>
<ellipse fill="none" stroke="black" cx="349.93" cy="-12.02" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="349.93" y="-10.12" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&#45;22 -->
<g id="edge15" class="edge">
<title>1&#45;&#45;22</title>
<path fill="none" stroke="black" d="M368.32,-165.66C364.64,-134.99 355.06,-54.92 351.39,-24.23"/>
</g>
<!-- 2&#45;&#45;3 -->
<g id="edge17" class="edge">
<title>2&#45;&#45;3</title>
<path fill="none" stroke="black" d="M472.46,-100.54C490.64,-111.63 518.66,-128.73 536.78,-139.79"/>
</g>
<!-- 2&#45;&#45;4 -->
<g id="edge18" class="edge">
<title>2&#45;&#45;4</title>
<path fill="none" stroke="black" d="M418.27,-84.66C418.14,-84.64 418.02,-84.63 417.9,-84.61"/>
</g>
<!-- 2&#45;&#45;14 -->
<g id="edge20" class="edge">
<title>2&#45;&#45;14</title>
<path fill="none" stroke="black" d="M485.66,-97.27C486.43,-97.46 487.2,-97.65 487.97,-97.84"/>
</g>
<!-- 2&#45;&#45;20 -->
<g id="edge22" class="edge">
<title>2&#45;&#45;20</title>
<path fill="none" stroke="black" d="M461.96,-101.68C474.33,-121.93 499.14,-162.56 511.45,-182.71"/>
</g>
<!-- 31 -->
<g id="node24" class="node">
<title>31</title>
<ellipse fill="none" stroke="black" cx="632.33" cy="-83.6" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="632.33" y="-81.7" font-family="Times,serif" font-size="8.00">0.024185</text>
</g>
<!-- 2&#45;&#45;31 -->
<g id="edge24" class="edge">
<title>2&#45;&#45;31</title>
<path fill="none" stroke="black" d="M494.05,-88.3C523.4,-87.3 563.29,-85.95 592.69,-84.95"/>
</g>
<!-- 2&#45;&#45;8 -->
<g id="edge19" class="edge">
<title>2&#45;&#45;8</title>
<path fill="none" stroke="black" d="M448.84,-101.74C443.47,-113 435.52,-129.66 430.23,-140.77"/>
</g>
<!-- 2&#45;&#45;18 -->
<g id="edge21" class="edge">
<title>2&#45;&#45;18</title>
<path fill="none" stroke="black" d="M414.8,-88.87C387.82,-88.34 353.36,-87.67 332.27,-87.26"/>
</g>
<!-- 2&#45;&#45;22 -->
<g id="edge23" class="edge">
<title>2&#45;&#45;22</title>
<path fill="none" stroke="black" d="M439.16,-78.19C418.35,-62.75 381.69,-35.57 362.48,-21.33"/>
</g>
<!-- 3&#45;&#45;4 -->
<g id="edge25" class="edge">
<title>3&#45;&#45;4</title>
<path fill="none" stroke="black" d="M532.31,-140.58C503.91,-127.75 454.95,-105.64 426.5,-92.79"/>
</g>
<!-- 3&#45;&#45;9 -->
<g id="edge27" class="edge">
<title>3&#45;&#45;9</title>
<path fill="none" stroke="black" d="M594.03,-149.06C594.17,-149.06 594.31,-149.05 594.45,-149.04"/>
</g>
<!-- 10 -->
<g id="node9" class="node">
<title>10</title>
<ellipse fill="none" stroke="black" cx="667.89" cy="-32.34" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="667.89" y="-30.44" font-family="Times,serif" font-size="8.00">0.010428</text>
</g>
<!-- 3&#45;&#45;10 -->
<g id="edge28" class="edge">
<title>3&#45;&#45;10</title>
<path fill="none" stroke="black" d="M565.77,-138.82C587.51,-116.16 634.94,-66.69 656.65,-44.06"/>
</g>
<!-- 3&#45;&#45;14 -->
<g id="edge29" class="edge">
<title>3&#45;&#45;14</title>
<path fill="none" stroke="black" d="M545.15,-138.73C539.97,-132.15 533.55,-123.99 528.36,-117.39"/>
</g>
<!-- 28 -->
<g id="node21" class="node">
<title>28</title>
<ellipse fill="none" stroke="black" cx="678.77" cy="-295.85" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="678.77" y="-293.95" font-family="Times,serif" font-size="8.00">0.025105</text>
</g>
<!-- 3&#45;&#45;28 -->
<g id="edge30" class="edge">
<title>3&#45;&#45;28</title>
<path fill="none" stroke="black" d="M564.65,-162.48C588.07,-189.86 645.59,-257.07 668.81,-284.2"/>
</g>
<!-- 29 -->
<g id="node22" class="node">
<title>29</title>
<ellipse fill="none" stroke="black" cx="650.65" cy="-224.3" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="650.65" y="-222.4" font-family="Times,serif" font-size="8.00">0.017155</text>
</g>
<!-- 3&#45;&#45;29 -->
<g id="edge31" class="edge">
<title>3&#45;&#45;29</title>
<path fill="none" stroke="black" d="M569.09,-161.79C587.16,-175.64 617.77,-199.1 635.91,-213"/>
</g>
<!-- 33 -->
<g id="node26" class="node">
<title>33</title>
<ellipse fill="none" stroke="black" cx="753.15" cy="-150.89" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="753.15" y="-148.99" font-family="Times,serif" font-size="8.00">0.097256</text>
</g>
<!-- 3&#45;&#45;33 -->
<g id="edge32" class="edge">
<title>3&#45;&#45;33</title>
<path fill="none" stroke="black" d="M594.21,-150.65C628.86,-150.71 678.86,-150.78 713.49,-150.83"/>
</g>
<!-- 3&#45;&#45;8 -->
<g id="edge26" class="edge">
<title>3&#45;&#45;8</title>
<path fill="none" stroke="black" d="M514.83,-151.15C489.78,-151.5 458.78,-151.93 440.43,-152.19"/>
</g>
<!-- 4&#45;&#45;14 -->
<g id="edge35" class="edge">
<title>4&#45;&#45;14</title>
<path fill="none" stroke="black" d="M437.42,-89.31C452.51,-92.3 470.33,-95.82 485.46,-98.82"/>
</g>
<!-- 4&#45;&#45;8 -->
<g id="edge33" class="edge">
<title>4&#45;&#45;8</title>
<path fill="none" stroke="black" d="M407.8,-94.88C411.55,-107.66 417.42,-127.68 421.16,-140.41"/>
</g>
<!-- 4&#45;&#45;13 -->
<g id="edge34" class="edge">
<title>4&#45;&#45;13</title>
<path fill="none" stroke="black" d="M374.99,-74.54C348.63,-67.16 310.72,-56.54 288.69,-50.36"/>
</g>
<!-- 5&#45;&#45;7 -->
<g id="edge36" class="edge">
<title>5&#45;&#45;7</title>
<path fill="none" stroke="black" d="M197.34,-199.76C187.74,-208.44 174.67,-220.26 165.08,-228.93"/>
</g>
<!-- 5&#45;&#45;11 -->
<g id="edge37" class="edge">
<title>5&#45;&#45;11</title>
<path fill="none" stroke="black" d="M212.15,-200.43C213.88,-211.34 216.4,-227.2 218.13,-238.1"/>
</g>
<!-- 6&#45;&#45;7 -->
<g id="edge38" class="edge">
<title>6&#45;&#45;7</title>
<path fill="none" stroke="black" d="M147.97,-207.99C148.8,-214.3 149.81,-221.99 150.64,-228.3"/>
</g>
<!-- 6&#45;&#45;11 -->
<g id="edge39" class="edge">
<title>6&#45;&#45;11</title>
<path fill="none" stroke="black" d="M161.64,-207.07C175.13,-217.07 194.58,-231.48 207.28,-240.89"/>
</g>
<!-- 17 -->
<g id="node33" class="node">
<title>17</title>
<ellipse fill="none" stroke="black" cx="19.09" cy="-239.97" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="19.09" y="-238.07" font-family="Times,serif" font-size="8.00">17</text>
</g>
<!-- 6&#45;&#45;17 -->
<g id="edge40" class="edge">
<title>6&#45;&#45;17</title>
<path fill="none" stroke="black" d="M120.3,-204.81C95.14,-213.55 57.77,-226.53 36.18,-234.04"/>
</g>
<!-- 7&#45;&#45;17 -->
<g id="edge41" class="edge">
<title>7&#45;&#45;17</title>
<path fill="none" stroke="black" d="M112.3,-240.36C87.78,-240.26 57.5,-240.13 38.24,-240.05"/>
</g>
<!-- 9&#45;&#45;31 -->
<g id="edge42" class="edge">
<title>9&#45;&#45;31</title>
<path fill="none" stroke="black" d="M604.07,-136.94C610.31,-125.15 619.79,-107.27 626.02,-95.51"/>
</g>
<!-- 9&#45;&#45;33 -->
<g id="edge43" class="edge">
<title>9&#45;&#45;33</title>
<path fill="none" stroke="black" d="M637.34,-149.42C660.64,-149.72 690,-150.09 713.33,-150.38"/>
</g>
<!-- 34 -->
<g id="node34" class="node">
<title>34</title>
<ellipse fill="none" stroke="black" cx="728.26" cy="-165.88" rx="19.18" ry="12.04"/>
<text text-anchor="middle" x="728.26" y="-163.98" font-family="Times,serif" font-size="8.00">34</text>
</g>
<!-- 9&#45;&#45;34 -->
<g id="edge44" class="edge">
<title>9&#45;&#45;34</title>
<path fill="none" stroke="black" d="M634.43,-153.69C658.86,-156.86 689.95,-160.9 709.48,-163.44"/>
</g>
<!-- 10&#45;&#45;34 -->
<g id="edge45" class="edge">
<title>10&#45;&#45;34</title>
<path fill="none" stroke="black" d="M673.46,-44.66C685.02,-70.23 711.48,-128.76 722.87,-153.97"/>
</g>
<!-- 14&#45;&#45;34 -->
<g id="edge46" class="edge">
<title>14&#45;&#45;34</title>
<path fill="none" stroke="black" d="M548.07,-113.85C592.21,-126.6 674.75,-150.43 710.73,-160.82"/>
</g>
<!-- 15 -->
<g id="node11" class="node">
<title>15</title>
<ellipse fill="none" stroke="black" cx="788.84" cy="-29.27" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="788.84" y="-27.37" font-family="Times,serif" font-size="8.00">0.011927</text>
</g>
<!-- 15&#45;&#45;33 -->
<g id="edge47" class="edge">
<title>15&#45;&#45;33</title>
<path fill="none" stroke="black" d="M785.29,-41.38C778.45,-64.69 763.52,-115.56 756.68,-138.83"/>
</g>
<!-- 15&#45;&#45;34 -->
<g id="edge48" class="edge">
<title>15&#45;&#45;34</title>
<path fill="none" stroke="black" d="M783.46,-41.4C771.94,-67.38 744.97,-128.19 733.54,-153.98"/>
</g>
<!-- 16 -->
<g id="node12" class="node">
<title>16</title>
<ellipse fill="none" stroke="black" cx="893.62" cy="-128.77" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="893.62" y="-126.87" font-family="Times,serif" font-size="8.00">0.011927</text>
</g>
<!-- 16&#45;&#45;33 -->
<g id="edge49" class="edge">
<title>16&#45;&#45;33</title>
<path fill="none" stroke="black" d="M858.17,-134.35C836.88,-137.71 809.87,-141.96 788.58,-145.31"/>
</g>
<!-- 16&#45;&#45;34 -->
<g id="edge50" class="edge">
<title>16&#45;&#45;34</title>
<path fill="none" stroke="black" d="M861.73,-135.93C827.38,-143.64 773.94,-155.63 746.25,-161.84"/>
</g>
<!-- 19 -->
<g id="node13" class="node">
<title>19</title>
<ellipse fill="none" stroke="black" cx="842.86" cy="-45.31" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="842.86" y="-43.41" font-family="Times,serif" font-size="8.00">0.011927</text>
</g>
<!-- 19&#45;&#45;33 -->
<g id="edge51" class="edge">
<title>19&#45;&#45;33</title>
<path fill="none" stroke="black" d="M832.91,-57.01C815.67,-77.31 780.41,-118.8 763.13,-139.13"/>
</g>
<!-- 19&#45;&#45;34 -->
<g id="edge52" class="edge">
<title>19&#45;&#45;34</title>
<path fill="none" stroke="black" d="M831.87,-56.87C809.57,-80.33 759.33,-133.19 738.06,-155.56"/>
</g>
<!-- 20&#45;&#45;34 -->
<g id="edge53" class="edge">
<title>20&#45;&#45;34</title>
<path fill="none" stroke="black" d="M555.22,-189.61C600.13,-183.45 674.68,-173.23 709.39,-168.47"/>
</g>
<!-- 21 -->
<g id="node15" class="node">
<title>21</title>
<ellipse fill="none" stroke="black" cx="856.16" cy="-94.83" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="856.16" y="-92.93" font-family="Times,serif" font-size="8.00">0.011927</text>
</g>
<!-- 21&#45;&#45;33 -->
<g id="edge54" class="edge">
<title>21&#45;&#45;33</title>
<path fill="none" stroke="black" d="M836.78,-105.38C818.48,-115.34 791.17,-130.2 772.79,-140.2"/>
</g>
<!-- 21&#45;&#45;34 -->
<g id="edge55" class="edge">
<title>21&#45;&#45;34</title>
<path fill="none" stroke="black" d="M837.01,-105.47C811.23,-119.79 765.91,-144.96 742.76,-157.82"/>
</g>
<!-- 23 -->
<g id="node16" class="node">
<title>23</title>
<ellipse fill="none" stroke="black" cx="871.39" cy="-179.11" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="871.39" y="-177.21" font-family="Times,serif" font-size="8.00">0.011927</text>
</g>
<!-- 23&#45;&#45;33 -->
<g id="edge56" class="edge">
<title>23&#45;&#45;33</title>
<path fill="none" stroke="black" d="M840.02,-171.62C822.91,-167.54 801.8,-162.5 784.67,-158.41"/>
</g>
<!-- 23&#45;&#45;34 -->
<g id="edge57" class="edge">
<title>23&#45;&#45;34</title>
<path fill="none" stroke="black" d="M833.41,-175.6C805.75,-173.04 769.43,-169.69 747.52,-167.66"/>
</g>
<!-- 24 -->
<g id="node17" class="node">
<title>24</title>
<ellipse fill="none" stroke="black" cx="777.53" cy="-294.45" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="777.53" y="-292.55" font-family="Times,serif" font-size="8.00">0.034980</text>
</g>
<!-- 26 -->
<g id="node19" class="node">
<title>26</title>
<ellipse fill="none" stroke="black" cx="722.31" cy="-410.86" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="722.31" y="-408.96" font-family="Times,serif" font-size="8.00">0.025816</text>
</g>
<!-- 24&#45;&#45;26 -->
<g id="edge58" class="edge">
<title>24&#45;&#45;26</title>
<path fill="none" stroke="black" d="M771.83,-306.47C761.24,-328.77 738.68,-376.34 728.06,-398.73"/>
</g>
<!-- 24&#45;&#45;28 -->
<g id="edge59" class="edge">
<title>24&#45;&#45;28</title>
<path fill="none" stroke="black" d="M737.93,-295.01C731.49,-295.1 724.82,-295.19 718.38,-295.28"/>
</g>
<!-- 30 -->
<g id="node23" class="node">
<title>30</title>
<ellipse fill="none" stroke="black" cx="843.66" cy="-246.79" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="843.66" y="-244.89" font-family="Times,serif" font-size="8.00">0.030442</text>
</g>
<!-- 24&#45;&#45;30 -->
<g id="edge60" class="edge">
<title>24&#45;&#45;30</title>
<path fill="none" stroke="black" d="M793.2,-283.15C803.74,-275.56 817.58,-265.58 828.1,-258"/>
</g>
<!-- 24&#45;&#45;33 -->
<g id="edge61" class="edge">
<title>24&#45;&#45;33</title>
<path fill="none" stroke="black" d="M775.45,-282.21C770.84,-255.08 759.82,-190.19 755.22,-163.1"/>
</g>
<!-- 24&#45;&#45;34 -->
<g id="edge62" class="edge">
<title>24&#45;&#45;34</title>
<path fill="none" stroke="black" d="M772.8,-282.12C763.31,-257.36 742.05,-201.87 732.75,-177.61"/>
</g>
<!-- 25 -->
<g id="node18" class="node">
<title>25</title>
<ellipse fill="none" stroke="black" cx="652.18" cy="-422.96" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="652.18" y="-421.06" font-family="Times,serif" font-size="8.00">0.025567</text>
</g>
<!-- 25&#45;&#45;26 -->
<g id="edge63" class="edge">
<title>25&#45;&#45;26</title>
<path fill="none" stroke="black" d="M686.83,-416.98C687.11,-416.93 687.38,-416.88 687.66,-416.84"/>
</g>
<!-- 25&#45;&#45;28 -->
<g id="edge64" class="edge">
<title>25&#45;&#45;28</title>
<path fill="none" stroke="black" d="M654.73,-410.77C659.8,-386.54 671.09,-332.54 676.19,-308.18"/>
</g>
<!-- 25&#45;&#45;32 -->
<g id="edge65" class="edge">
<title>25&#45;&#45;32</title>
<path fill="none" stroke="black" d="M648.78,-410.74C641.24,-383.66 623.22,-318.87 615.7,-291.82"/>
</g>
<!-- 26&#45;&#45;32 -->
<g id="edge66" class="edge">
<title>26&#45;&#45;32</title>
<path fill="none" stroke="black" d="M712.15,-398.75C691.09,-373.62 642.88,-316.1 622.12,-291.33"/>
</g>
<!-- 27 -->
<g id="node20" class="node">
<title>27</title>
<ellipse fill="none" stroke="black" cx="915.73" cy="-254.29" rx="39.7" ry="12.04"/>
<text text-anchor="middle" x="915.73" y="-252.39" font-family="Times,serif" font-size="8.00">0.013037</text>
</g>
<!-- 27&#45;&#45;30 -->
<g id="edge67" class="edge">
<title>27&#45;&#45;30</title>
<path fill="none" stroke="black" d="M878.01,-250.36C877.87,-250.35 877.74,-250.34 877.6,-250.32"/>
</g>
<!-- 27&#45;&#45;34 -->
<g id="edge68" class="edge">
<title>27&#45;&#45;34</title>
<path fill="none" stroke="black" d="M894.23,-244.15C855.95,-226.1 776.89,-188.81 743.59,-173.11"/>
</g>
<!-- 28&#45;&#45;34 -->
<g id="edge69" class="edge">
<title>28&#45;&#45;34</title>
<path fill="none" stroke="black" d="M683.34,-283.85C692.77,-259.09 714.28,-202.58 723.7,-177.85"/>
</g>
<!-- 29&#45;&#45;32 -->
<g id="edge70" class="edge">
<title>29&#45;&#45;32</title>
<path fill="none" stroke="black" d="M642.33,-236.31C635.9,-245.59 627.04,-258.37 620.61,-267.65"/>
</g>
<!-- 29&#45;&#45;34 -->
<g id="edge71" class="edge">
<title>29&#45;&#45;34</title>
<path fill="none" stroke="black" d="M665.62,-213.04C680.24,-202.03 702.24,-185.47 715.9,-175.19"/>
</g>
<!-- 30&#45;&#45;33 -->
<g id="edge72" class="edge">
<title>30&#45;&#45;33</title>
<path fill="none" stroke="black" d="M832.56,-235.03C815.1,-216.52 781.4,-180.82 764.05,-162.45"/>
</g>
<!-- 30&#45;&#45;34 -->
<g id="edge73" class="edge">
<title>30&#45;&#45;34</title>
<path fill="none" stroke="black" d="M827.62,-235.54C804.58,-219.39 762.57,-189.93 741.31,-175.03"/>
</g>
<!-- 31&#45;&#45;33 -->
<g id="edge74" class="edge">
<title>31&#45;&#45;33</title>
<path fill="none" stroke="black" d="M651.47,-94.27C674.09,-106.86 711.51,-127.7 734.09,-140.28"/>
</g>
<!-- 31&#45;&#45;34 -->
<g id="edge75" class="edge">
<title>31&#45;&#45;34</title>
<path fill="none" stroke="black" d="M645.67,-95.04C664.61,-111.29 698.99,-140.78 716.83,-156.08"/>
</g>
<!-- 32&#45;&#45;33 -->
<g id="edge76" class="edge">
<title>32&#45;&#45;33</title>
<path fill="none" stroke="black" d="M624.81,-268.2C651.47,-243.83 713.66,-186.98 740.47,-162.47"/>
</g>
<!-- 32&#45;&#45;34 -->
<g id="edge77" class="edge">
<title>32&#45;&#45;34</title>
<path fill="none" stroke="black" d="M624.28,-267.88C647.08,-245.52 696.36,-197.17 717.87,-176.07"/>
</g>
<!-- 33&#45;&#45;34 -->
<g id="edge78" class="edge">
<title>33&#45;&#45;34</title>
<path fill="none" stroke="black" d="M735.32,-161.62C735.2,-161.7 735.08,-161.77 734.95,-161.85"/>
</g>
</g>
</svg>
</div>
