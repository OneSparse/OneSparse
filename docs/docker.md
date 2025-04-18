# Quick Start with Docker

Start the onesparse demo docker container, substitute suitable
password and container name:

```
docker run -d -e POSTGRES_PASSWORD=password -it --name onesparse-demo onesparse/onesparse:demo
```

The container will start detached in the background, verify it's
running with 'docker ps'.  Now exec a psql process to interact with
the container:

```
docker exec -u postgres -it onesparse-demo psql

postgres# create extension onesparse;
CREATE EXTENSION
postgres# select print(random_matrix(8, 8, 16, seed=>0.42, max=>42)) as random_matrix;
┌────────────────────────────────┐
│         random_matrix          │
├────────────────────────────────┤
│      0  1  2  3  4  5  6  7   ↵│
│    ────────────────────────   ↵│
│  0│              6 31         ↵│
│  1│                   12      ↵│
│  2│    26                     ↵│
│  3│                40         ↵│
│  4│    11    15           7   ↵│
│  5│ 20 12    22        1      ↵│
│  6│              1            ↵│
│  7│       17                  ↵│
│                                │
└────────────────────────────────┘
(1 row)
```

This matrix represents the following graph:

<div>
<!-- Title: %3 Pages: 1 -->
<svg width="245pt" height="377pt"
 viewBox="0.00 0.00 245.24 377.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(4 373.25)">
<title>%3</title>
<polygon fill="white" stroke="transparent" points="-4,4 -4,-373.25 241.24,-373.25 241.24,4 -4,4"/>
<!-- 0 -->
<g id="node1" class="node">
<title>0</title>
<ellipse fill="none" stroke="black" cx="34.24" cy="-357.23" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="34.24" y="-355.33" font-family="Times,serif" font-size="8.00">0</text>
</g>
<!-- 4 -->
<g id="node2" class="node">
<title>4</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-288.19" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-286.29" font-family="Times,serif" font-size="8.00">4</text>
</g>
<!-- 0&#45;&gt;4 -->
<g id="edge1" class="edge">
<title>0&#45;&gt;4</title>
<path fill="none" stroke="black" d="M45.44,-348.8C62.35,-337.57 94.65,-316.13 115.55,-302.26"/>
<polygon fill="black" stroke="black" points="117.6,-305.1 124,-296.65 113.73,-299.27 117.6,-305.1"/>
<text text-anchor="middle" x="94.24" y="-320.81" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5 -->
<g id="node3" class="node">
<title>5</title>
<ellipse fill="none" stroke="black" cx="71.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="71.24" y="-148.2" font-family="Times,serif" font-size="8.00">5</text>
</g>
<!-- 0&#45;&gt;5 -->
<g id="edge2" class="edge">
<title>0&#45;&gt;5</title>
<path fill="none" stroke="black" d="M27.98,-346.15C17.09,-327.46 -3.73,-285.98 1.24,-249.17 5.5,-217.63 0.71,-205.25 20.24,-180.12 27.28,-171.08 37.98,-164.38 47.77,-159.71"/>
<polygon fill="black" stroke="black" points="49.27,-162.87 57.08,-155.71 46.51,-156.44 49.27,-162.87"/>
<text text-anchor="middle" x="6.74" y="-251.77" font-family="Times,serif" font-size="8.00">31</text>
</g>
<!-- 1 -->
<g id="node4" class="node">
<title>1</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-81.06" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 4&#45;&gt;1 -->
<g id="edge6" class="edge">
<title>4&#45;&gt;1</title>
<path fill="none" stroke="black" d="M135.24,-276.16C135.24,-243.46 135.24,-146.65 135.24,-103.38"/>
<polygon fill="black" stroke="black" points="138.74,-103.15 135.24,-93.15 131.74,-103.15 138.74,-103.15"/>
<text text-anchor="middle" x="140.74" y="-182.72" font-family="Times,serif" font-size="8.00">11</text>
</g>
<!-- 3 -->
<g id="node7" class="node">
<title>3</title>
<ellipse fill="none" stroke="black" cx="91.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="91.24" y="-217.25" font-family="Times,serif" font-size="8.00">3</text>
</g>
<!-- 4&#45;&gt;3 -->
<g id="edge7" class="edge">
<title>4&#45;&gt;3</title>
<path fill="none" stroke="black" d="M126.28,-277.99C121.21,-272.51 114.96,-265.25 110.24,-258.17 106.52,-252.58 103.05,-246.16 100.11,-240.21"/>
<polygon fill="black" stroke="black" points="103.15,-238.43 95.74,-230.86 96.81,-241.4 103.15,-238.43"/>
<text text-anchor="middle" x="115.74" y="-251.77" font-family="Times,serif" font-size="8.00">15</text>
</g>
<!-- 7 -->
<g id="node8" class="node">
<title>7</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-219.15" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-217.25" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 4&#45;&gt;7 -->
<g id="edge8" class="edge">
<title>4&#45;&gt;7</title>
<path fill="none" stroke="black" d="M141.93,-277.01C148.52,-266.96 158.75,-251.38 166.83,-239.06"/>
<polygon fill="black" stroke="black" points="169.94,-240.7 172.5,-230.42 164.08,-236.86 169.94,-240.7"/>
<text text-anchor="middle" x="163.24" y="-251.77" font-family="Times,serif" font-size="8.00">7</text>
</g>
<!-- 5&#45;&gt;0 -->
<g id="edge9" class="edge">
<title>5&#45;&gt;0</title>
<path fill="none" stroke="black" d="M67.93,-162.06C66.37,-167.46 64.57,-174.1 63.24,-180.12 51.07,-235.59 41.46,-301.9 36.99,-335.1"/>
<polygon fill="black" stroke="black" points="33.52,-334.65 35.67,-345.02 40.45,-335.57 33.52,-334.65"/>
<text text-anchor="middle" x="55.74" y="-251.77" font-family="Times,serif" font-size="8.00">20</text>
</g>
<!-- 5&#45;&gt;1 -->
<g id="edge10" class="edge">
<title>5&#45;&gt;1</title>
<path fill="none" stroke="black" d="M79.87,-140.06C89.93,-129.53 106.77,-111.89 119.24,-98.83"/>
<polygon fill="black" stroke="black" points="121.96,-101.04 126.34,-91.39 116.9,-96.21 121.96,-101.04"/>
<text text-anchor="middle" x="112.74" y="-113.68" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6 -->
<g id="node5" class="node">
<title>6</title>
<ellipse fill="none" stroke="black" cx="135.24" cy="-12.02" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="135.24" y="-10.12" font-family="Times,serif" font-size="8.00">6</text>
</g>
<!-- 5&#45;&gt;6 -->
<g id="edge12" class="edge">
<title>5&#45;&gt;6</title>
<path fill="none" stroke="black" d="M75.24,-138.31C81.09,-122.77 92.58,-93.29 104.24,-69.04 110.29,-56.46 117.86,-42.75 124.01,-32.03"/>
<polygon fill="black" stroke="black" points="127.17,-33.56 129.17,-23.16 121.12,-30.05 127.17,-33.56"/>
<text text-anchor="middle" x="107.24" y="-79.16" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 5&#45;&gt;3 -->
<g id="edge11" class="edge">
<title>5&#45;&gt;3</title>
<path fill="none" stroke="black" d="M74.52,-162.07C77.37,-171.66 81.6,-185.83 85.09,-197.53"/>
<polygon fill="black" stroke="black" points="81.82,-198.79 88.03,-207.37 88.53,-196.79 81.82,-198.79"/>
<text text-anchor="middle" x="88.74" y="-182.72" font-family="Times,serif" font-size="8.00">22</text>
</g>
<!-- 1&#45;&gt;6 -->
<g id="edge3" class="edge">
<title>1&#45;&gt;6</title>
<path fill="none" stroke="black" d="M135.24,-68.99C135.24,-59.63 135.24,-45.96 135.24,-34.47"/>
<polygon fill="black" stroke="black" points="138.74,-34.33 135.24,-24.33 131.74,-34.33 138.74,-34.33"/>
<text text-anchor="middle" x="140.74" y="-44.64" font-family="Times,serif" font-size="8.00">12</text>
</g>
<!-- 6&#45;&gt;4 -->
<g id="edge13" class="edge">
<title>6&#45;&gt;4</title>
<path fill="none" stroke="black" d="M150.24,-16.18C177.24,-22.84 231.24,-41.03 231.24,-80.06 231.24,-220.15 231.24,-220.15 231.24,-220.15 231.24,-254.91 188.41,-273.14 159.97,-281.42"/>
<polygon fill="black" stroke="black" points="158.99,-278.06 150.24,-284.03 160.81,-284.82 158.99,-278.06"/>
<text text-anchor="middle" x="234.24" y="-148.2" font-family="Times,serif" font-size="8.00">1</text>
</g>
<!-- 2 -->
<g id="node6" class="node">
<title>2</title>
<ellipse fill="none" stroke="black" cx="179.24" cy="-150.1" rx="15.61" ry="12.04"/>
<text text-anchor="middle" x="179.24" y="-148.2" font-family="Times,serif" font-size="8.00">2</text>
</g>
<!-- 2&#45;&gt;1 -->
<g id="edge4" class="edge">
<title>2&#45;&gt;1</title>
<path fill="none" stroke="black" d="M172.56,-138.92C165.97,-128.88 155.74,-113.29 147.66,-100.98"/>
<polygon fill="black" stroke="black" points="150.4,-98.78 141.99,-92.34 144.55,-102.62 150.4,-98.78"/>
<text text-anchor="middle" x="165.74" y="-113.68" font-family="Times,serif" font-size="8.00">26</text>
</g>
<!-- 3&#45;&gt;5 -->
<g id="edge5" class="edge">
<title>3&#45;&gt;5</title>
<path fill="none" stroke="black" d="M81.77,-209.46C76.7,-204.15 70.95,-196.9 68.24,-189.12 66.39,-183.79 66.1,-177.79 66.54,-172.16"/>
<polygon fill="black" stroke="black" points="70.04,-172.47 68.08,-162.06 63.12,-171.42 70.04,-172.47"/>
<text text-anchor="middle" x="73.74" y="-182.72" font-family="Times,serif" font-size="8.00">40</text>
</g>
<!-- 7&#45;&gt;2 -->
<g id="edge14" class="edge">
<title>7&#45;&gt;2</title>
<path fill="none" stroke="black" d="M179.24,-207.08C179.24,-197.72 179.24,-184.04 179.24,-172.55"/>
<polygon fill="black" stroke="black" points="182.74,-172.42 179.24,-162.42 175.74,-172.42 182.74,-172.42"/>
<text text-anchor="middle" x="184.74" y="-182.72" font-family="Times,serif" font-size="8.00">17</text>
</g>
</g>
</svg>
</div>

Check out the [Getting Started](test_examples_header.md) for some
examples and the [Matrix Documentation](test_matrix_header.md).  You
can verify the doctests with docker using:

```
$ docker exec -u postgres -it onesparse-demo-container make installcheck
```
