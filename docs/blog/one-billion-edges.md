---
title: One Billion Edges Per Second with PostgreSQL
date: 2024-01-01
description: Outline for a blog post about using OneSparse with Postgres to perform extremely fast edge traversals.
---

# One Billion Edges Per Second with PostgreSQL

This post outlines how OneSparse leverages GraphBLAS inside PostgreSQL to traverse graphs with billions of edges at blazingly fast rates.

## Outline

- Introduction to graph analytics in Postgres
- Overview of the OneSparse extension and GraphBLAS
- Loading large graph datasets with billions of edges
- Running breadth-first search and other traversals using OneSparse
- Achieving billions of edges per second: hardware and configuration tips
- Benchmarks and performance results
- Conclusion and future work
