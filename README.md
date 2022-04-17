# Apollo ranging

During the Moon missions, the distance to the Apollo spacecraft was determined by measuring the time delay of a
signal to the spacecraft and back. This signal was a pseudo-random sequence. By correlating the transmitted sequence
with the received sequence, the delay could be accurately measured even with a weak, noisy signal.

This program generates the pseudo-random bit sequence using the original algorithm. The sequence consists of multiple,
short sub-codes, which could be generated and correlated using the technology of the 1960s. The sub-codes were
combined in a special way so the sub-codes could still be recognized in the overall sequence.
The resulting sequence was transmitted at 1 MHz, and was 5,456,682 bits long. The sequence took over 5 seconds
to repeat, enough time for the signal to reach the Moon and back, and provide an unambiguous range.

See [A study of the JPL Mark I Ranging Subsystem](https://ntrs.nasa.gov/citations/19660004934)
