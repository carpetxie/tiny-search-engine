#!/bin/bash
# testing.sh - test script for querier

# paths to local test data
PAGEDIR="./toscrape-1"
INDEXFILE="./toscrape-1.index"

echo "=== Test: invalid number of arguments ==="
./querier
./querier one
./querier one two three

echo
echo "=== Test: invalid pageDirectory ==="
./querier /nonexistent "$INDEXFILE"

echo
echo "=== Test: invalid indexFilename ==="
./querier "$PAGEDIR" /nonexistent

echo
echo "=== Test: valid inputs - simple query ==="
echo "home" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: AND operator ==="
echo "search and engine" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: OR operator ==="
echo "home or search" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: AND precedence over OR ==="
echo "computer science or programming language" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: error - bad character ==="
echo "hello!" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: error - 'and' at start ==="
echo "and hello" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: error - 'or' at end ==="
echo "hello or" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: error - adjacent operators ==="
echo "hello and or world" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Test: empty query ==="
echo "" | ./querier "$PAGEDIR" "$INDEXFILE"

echo
echo "=== Testing complete ==="