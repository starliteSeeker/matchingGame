# matchingGame

A card matching game that rigs the card value so you get the worst possible outcome :)

```text
                                                                          *
                   ┌───────────────┐                   ┌───────────────┐  * guesses:
                   │               │                   │               │  *         5
                   │               │                   │               │  *
                   │               │                   │               │  *
                   └───────────────┘                   └───────────────┘  *
                                    ┌                 ┐                   *
                   ┌───────────────┐ ┌───────────────┐ ┌───────────────┐  *
                   │               │ │E6             │ │               │  *
                   │               │ │               │ │               │  *
                   │               │ │               │ │               │  * space
                   └───────────────┘ └───────────────┘ └───────────────┘  * to select
                                    └                 ┘                   *
 ┌───────────────┐ ┌───────────────┐ ┌───────────────┐                    * w/a/s/d
 │               │ │               │ │               │                    * to move
 │               │ │               │ │               │                    *
 │               │ │               │ │               │                    * q
 └───────────────┘ └───────────────┘ └───────────────┘                    * to quit
                                                                          *

```

## Algorithm

For the first flip, it tries to show you a card value not yet seen. For the second flip, it tries to show you a card you've seen before (so you need another turn to pair up those cards).
