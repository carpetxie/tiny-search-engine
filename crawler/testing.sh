#!/bin/bash
# testing.sh - tests for TSE crawler
# Usage: bash -v testing.sh &> testing.out

# Build the crawler
echo "### Building crawler"
make clean
make

CRAWLER=./crawler

# TSE seed URLs
LETTERS="http://cs50tse.cs.dartmouth.edu/tse/letters/index.html"
TOSCRAPE="http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html"
WIKI="http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html"

# Create pageDirectories (must exist before running crawler)
mkdir -p ../data/letters-0
mkdir -p ../data/letters-1
mkdir -p ../data/toscrape-0
mkdir -p ../data/toscrape-1
mkdir -p ../data/wiki-0
mkdir -p ../data/wiki-1

echo
echo "### Part 1: bad argument tests"

echo "1) Too few arguments"
$CRAWLER
echo

echo "2) Non-integer maxDepth"
$CRAWLER "$LETTERS" ../data/letters-0 notAnInt
echo

echo "3) Negative maxDepth"
$CRAWLER "$LETTERS" ../data/letters-0 -1
echo

echo "4) Too large maxDepth"
$CRAWLER "$LETTERS" ../data/letters-0 20
echo

echo "5) External seed URL"
$CRAWLER "http://www.google.com" ../data/letters-0 1
echo

echo "6) Non-existent pageDirectory"
$CRAWLER "$LETTERS" ../data/noSuchDir 1
echo

echo
echo "### Part 2: small valid crawls"

echo "7) letters at depth 0"
$CRAWLER "$LETTERS" ../data/letters-0 0
echo

echo "8) letters at depth 1"
$CRAWLER "$LETTERS" ../data/letters-1 1
echo

echo "9) toscrape at depth 0"
$CRAWLER "$TOSCRAPE" ../data/toscrape-1 0
echo

echo "10) toscrape at depth 1"
$CRAWLER "$TOSCRAPE" ../data/toscrape-1 1
echo

echo "11) wikipedia at depth 0"
$CRAWLER "$WIKI" ../data/wiki-0 0
echo

echo "12) wikipedia at depth 1"
$CRAWLER "$WIKI" ../data/wiki-1 1
echo

echo "### Valgrind - see Makefile, run separately 'make valgrind'"

echo "### Done testing."