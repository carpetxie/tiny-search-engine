# Querier

## Functionality Implemented

This querier implementation meets the complete specification:
- Prints the set of documents containing all words in the query
- Supports 'and' and 'or' operators with proper precedence (AND before OR)
- Prints results in decreasing order by score

## Assumptions

- We assume the two input directories are VALID inputs 
- Queries contain only letters and spaces; all other characters are rejected.
- Words shorter than 3 characters are accepted in queries (even if indexer ignores them).
- Empty queries (blank lines) are silently ignored.

## Differences from Spec

None. The implementation follows the Requirements Spec exactly.

## Known Limitations

None known. All test cases pass.

