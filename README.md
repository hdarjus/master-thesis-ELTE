# Verifiable delay function implementations
I implement the VDF protocols from Wesolowski [1] and Pietrzak [2] in C/C++, using both OpenSSL and GNU MP for handling large integers.
I employ OpenSSL for its cryptographic features and GNU MP for its more user-friendly RNG.

The random oracle that hashes into a large set of prime numbers -- needed in the Wesolowski protocol -- is implemented using acceptance-rejection sampling.

## References
[1] Wesolowski, Benjamin. "Efficient verifiable delay functions." IACR Cryptology ePrint Archive 2018 (2018): 623.

[2] Pietrzak, Krzysztof. "Simple verifiable delay functions." 10th Innovations in Theoretical Computer Science Conference (ITCS 2019). Schloss Dagstuhl-Leibniz-Zentrum fuer Informatik, 2018.
