/*
The MIT License (MIT)

Copyright (c) 2014 Master.G

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "analysis.h"
#include "beat.h"

/*
 * extract hands like 34567 / 334455 / 333444555 etc
 * array is a processed card array holds count[rank] == duplicate
 */
static void
HandList_ExtractConsecutive(rk_list_t *hl, card_array_t *array, int duplicate) {
  int i = 0;
  int j = 0;
  int k = 0;
  int cardnum = 0;
  uint8_t lastrank = 0;
  hand_t hand;
  int primal[] = {0, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
  int chainlen[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH,
      HAND_TRIO_CHAIN_MIN_LENGTH};

  if ((duplicate < 1) || (duplicate > 3) || (array->length == 0))
    return;

  Hand_Clear(&hand);

  cardnum = array->length / duplicate - 1;
  lastrank = CARD_RANK(array->cards[0]);
  i = duplicate;

  while (cardnum--) {
    if ((lastrank - 1) != CARD_RANK(array->cards[i])) {
      /* chain break */
      if (i >= chainlen[duplicate]) {
        /* chain */
        Hand_Clear(&hand);
        hand.type =
            Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAIN);

        for (j = 0; j < i; j++)
          CardArray_PushBack(&hand.cards, CardArray_PopFront(array));

        HandList_PushFront(hl, &hand);
      } else {
        /* not a chain */
        for (j = 0; j < i / duplicate; j++) {
          Hand_Clear(&hand);
          hand.type =
              Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAINLESS);

          for (k = 0; k < duplicate; k++)
            CardArray_PushBack(&hand.cards, CardArray_PopFront(array));

          HandList_PushFront(hl, &hand);
        }
      }

      if (array->length == 0)
        break;

      lastrank = CARD_RANK(array->cards[0]);
      i = duplicate;
    } else {
      /* chain intact */
      lastrank = CARD_RANK(array->cards[i]);
      i += duplicate;
    }
  }

  /* all chained up */
  if ((i != 0) && (i == array->length)) {
    /* can chain up */
    if (i >= chainlen[duplicate]) {
      Hand_Clear(&hand);
      hand.type = Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAIN);

      for (j = 0; j < i; j++)
        CardArray_PushBack(&hand.cards, CardArray_PopFront(array));

      HandList_PushFront(hl, &hand);
    } else {
      for (j = 0; j < i / duplicate; j++) {
        Hand_Clear(&hand);
        hand.type =
            Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAINLESS);

        for (k = 0; k < duplicate; k++)
          CardArray_PushBack(&hand.cards, CardArray_PopFront(array));

        HandList_PushFront(hl, &hand);
      }
    }
  }
}

/* extract nuke/bomb/2 from array, these cards will be removed from array */
static void
HandList_ExtractNukeBomb2(rk_list_t *hl, card_array_t *array, int *count) {
  int i = 0;
  hand_t hand;

  /* nuke */
  if (count[CARD_RANK_r] && count[CARD_RANK_R]) {
    Hand_Clear(&hand);
    hand.type = Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_R);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_r);

    HandList_PushFront(hl, &hand);

    count[CARD_RANK_r] = 0;
    count[CARD_RANK_R] = 0;

    CardArray_RemoveRank(array, CARD_RANK_r);
    CardArray_RemoveRank(array, CARD_RANK_R);
  }

  /* bomb */
  for (i = CARD_RANK_2; i >= CARD_RANK_3; i--) {
    if (count[i] == 4) {
      Hand_Clear(&hand);
      hand.type =
          Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS);
      CardArray_CopyRank(&hand.cards, array, (uint8_t)i);

      HandList_PushFront(hl, &hand);

      count[i] = 0;
      CardArray_RemoveRank(array, (uint8_t)i);
    }
  }

  /* joker */
  if ((count[CARD_RANK_r] != 0) || (count[CARD_RANK_R] != 0)) {
    Hand_Clear(&hand);
    CardArray_CopyRank(
        &hand.cards, array,
        count[CARD_RANK_r] != 0 ? CARD_RANK_r : CARD_RANK_R);
    hand.type = Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS);

    HandList_PushFront(hl, &hand);
    count[CARD_RANK_r] = 0;
    count[CARD_RANK_R] = 0;
    CardArray_RemoveRank(array, CARD_RANK_r);
    CardArray_RemoveRank(array, CARD_RANK_R);
  }

  /* 2 */
  if (count[CARD_RANK_2] != 0) {
    Hand_Clear(&hand);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_2);

    switch (count[CARD_RANK_2]) {
    case 1:
      hand.type =
          Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    case 2:
      hand.type =
          Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    case 3:
      hand.type =
          Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    default:
      break;
    }
    count[CARD_RANK_2] = 0;
    CardArray_RemoveRank(array, CARD_RANK_2);
    HandList_PushFront(hl, &hand);
  }
}

rk_list_t *Analysis_Standard(card_array_t *cards) {
  int i = 0;
  int count[CARD_RANK_END];
  rk_list_t *hl = NULL;

  card_array_t array;
  card_array_t arrsolo;
  card_array_t arrpair;
  card_array_t arrtrio;
  card_array_t *arrprimals[3];

  CardArray_Clear(&arrsolo);
  CardArray_Clear(&arrpair);
  CardArray_Clear(&arrtrio);

  arrprimals[0] = &arrsolo;
  arrprimals[1] = &arrpair;
  arrprimals[2] = &arrtrio;

  CardArray_Copy(&array, cards);

  CardArray_Sort(&array, NULL);
  Hand_CountRank(&array, count);

  hl = rk_list_create();

  /* nuke, bomb and 2 */
  HandList_ExtractNukeBomb2(hl, &array, count);

  /* chains */
  for (i = 0; i < array.length;) {
    int c = count[CARD_RANK(array.cards[i])];

    if (c != 0) {
      CardArray_PushBackCards(arrprimals[c - 1], &array, i, c);
      i += c;
    } else {
      i++;
    }
  }

  /* chain */
  HandList_ExtractConsecutive(hl, &arrtrio, 3);
  HandList_ExtractConsecutive(hl, &arrpair, 2);
  HandList_ExtractConsecutive(hl, &arrsolo, 1);

  return hl;
}

int Analysis_CountHands(Analysis_Func analyze, card_array_t *array) {
  rk_list_t *hl = analyze(array);
  int hands = rk_list_count(hl);

  rk_list_clear_destroy(hl);

  return hands;
}

/*
 * ************************************************************
 * advanced analysis
 * ************************************************************
 */

/* cards being taken apart */
typedef struct hand_ctx_s {
  /* rank count */
  int count[CARD_RANK_END];
  /* original cards */
  card_array_t cards;
  /* reverse sorted cards */
  card_array_t rcards;

} hand_ctx_t;

#define HandCtx_Clear(ctx) memset((ctx), 0, sizeof(hand_ctx_t))

static void HandList_SearchLongestConsecutive(
    hand_ctx_t *ctx, hand_t *hand, int duplicate) {
  /* context */
  int i = 0;
  int j = 0;
  int k = 0;
  int rankstart = 0;
  uint8_t lastrank = 0;
  int primal[] = {0, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
  int chainlen[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH,
      HAND_TRIO_CHAIN_MIN_LENGTH};
  card_array_t chain;
  int *count = ctx->count;
  card_array_t *cards = &ctx->rcards;

  if ((duplicate < 1) || (duplicate > 3))
    return;

  /* early break */
  if (cards->length < chainlen[duplicate])
    return;

  /* setup */
  CardArray_Clear(&chain);

  Hand_Clear(hand);
  rankstart = 0;

  /*
   * i <= CARD_RANK_2
   * but count[CARD_RANK_2] must be 0
   * for 2/bomb/nuke has been removed before calling this function
   */
  for (i = CARD_RANK_3; i <= CARD_RANK_2; i++) {
    /* find start of a possible chain */
    if (rankstart == 0) {
      if (count[i] >= duplicate)
        rankstart = i;

      continue;
    }

    /* chain break */
    if (count[i] < duplicate) {
      /* chain break, extract chain and set new possible start */
      if ((((i - rankstart) * duplicate) >= chainlen[duplicate]) &&
          ((i - rankstart) > chain.length)) {
        /* valid chain, store rank in card_array_t */
        CardArray_Clear(&chain);

        for (j = rankstart; j < i; j++)
          CardArray_PushBack(&chain, (uint8_t)j);
      }

      rankstart = 0;
    }
  }

  /* convert rank array to card array */
  if (chain.length > 0) {
    for (i = chain.length - 1; i >= 0; i--) {
      lastrank = chain.cards[i];
      k = duplicate;

      for (j = 0; j < cards->length; j++) {
        if (CARD_RANK(cards->cards[j]) == lastrank) {
          CardArray_PushBack(&hand->cards, cards->cards[j]);
          k--;

          if (k == 0)
            break;
        }
      }
    }

    hand->type = Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAIN);
  }
}

typedef void (*HandList_SearchPrimalFunc)(hand_ctx_t *, hand_t *, int);

#define HAND_SEARCH_TYPES 3

/*
 * pass a empty hand to start traverse
 * result stores in hand
 * return 0 when stop
 */
static int HLAA_TraverseChains(hand_ctx_t *ctx, int *begin, hand_t *hand) {
  int found = 0;
  int i = *begin;
  int primals[] = {1, 2, 3};

  /* solo chain, pair chain, trio chain, trio, pair, solo */
  HandList_SearchPrimalFunc searchers[HAND_SEARCH_TYPES];

  searchers[0] = HandList_SearchLongestConsecutive;
  searchers[1] = HandList_SearchLongestConsecutive;
  searchers[2] = HandList_SearchLongestConsecutive;

  if (ctx->cards.length == 0)
    return 0;

  if (*begin >= HAND_SEARCH_TYPES)
    return 0;

  /* init search */
  if (hand->type == 0) {
    while (i < HAND_SEARCH_TYPES && hand->type == 0) {
      searchers[i](ctx, hand, primals[i]);

      if (hand->type != 0) {
        found = 1;
        break;
      } else {
        i++;
        *begin = i;
      }
    }

    /* if found == 0, should PANIC */
  } else {
    /* continue search via beat */
    found = Beat_Search(&ctx->cards, hand, hand);
  }

  return found;
}

/*
 * extract all chains or primal hands in hand_ctx
 */
static void HLAA_ExtractAllChains(hand_ctx_t *ctx, rk_list_t *hands) {
  int found = 0;
  int lastsearch = 0;
  hand_t workinghand;
  hand_t lasthand;

  /* init search */
  Hand_Clear(&workinghand);
  Hand_Clear(&lasthand);

  found = HLAA_TraverseChains(ctx, &lastsearch, &lasthand);

  while (found != 0) {
    HandList_PushFront(hands, &lasthand);

    Hand_Copy(&workinghand, &lasthand);

    while ((found = HLAA_TraverseChains(ctx, &lastsearch, &workinghand)) != 0)
      HandList_PushFront(hands, &workinghand);

    /* can't find any more hands, try to reduce chain length */
    if (lasthand.type != 0) {
      if (lasthand.type ==
          Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (lasthand.cards.length > HAND_SOLO_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 1);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      } else if (
          lasthand.type ==
          Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (lasthand.cards.length > HAND_PAIR_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 2);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      } else if (
          lasthand.type ==
          Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (lasthand.cards.length > HAND_TRIO_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 3);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      }

      /* still can't found, loop through hand type for more */
      if (found == 0) {
        lastsearch++;
        Hand_Clear(&lasthand);
        found = HLAA_TraverseChains(ctx, &lastsearch, &lasthand);
      }
    }
  }
}

/* advanced search tree payload */
typedef struct hltree_payload_s {
  /* hand context */
  hand_ctx_t ctx;
  /* hand */
  hand_t hand;
  /* evaluation weight */
  int weight;

} hltree_payload_t;

static rk_tree_t *HLAA_TreeAddHand(rk_tree_t *tree, rk_list_node_t *handnode) {
  hltree_payload_t *oldpayload = NULL;
  hltree_payload_t *newpayload = NULL;

  oldpayload = (hltree_payload_t *)tree->payload;
  newpayload = (hltree_payload_t *)malloc(sizeof(hltree_payload_t));

  /* make diff here */
  memcpy(&newpayload->ctx, &oldpayload->ctx, sizeof(hand_ctx_t));
  Hand_Copy(&newpayload->hand, HandList_GetHand(handnode));
  CardArray_Subtract(
      &newpayload->ctx.cards, &HandList_GetHand(handnode)->cards);
  CardArray_Copy(&newpayload->ctx.rcards, &newpayload->ctx.cards);
  CardArray_Reverse(&newpayload->ctx.rcards);
  Hand_CountRank(&newpayload->ctx.cards, newpayload->ctx.count);
  newpayload->weight = oldpayload->weight + 1;

  /* expand the tree */
  return rk_tree_add_child(tree, newpayload);
}

/*
 * search hand via least hands
 */
rk_list_t *Analysis_Advanced(card_array_t *array) {
  rk_list_t *handlist = NULL;
  rk_list_t *chains = NULL;
  rk_list_t *others = NULL;
  rk_list_node_t *hlnode = NULL;
  rk_list_t *st = NULL;
  rk_tree_t *grandtree = NULL;
  rk_tree_t *workingtree = NULL;
  rk_tree_t *tnode = NULL;
  rk_tree_t *shortest = NULL;
  hltree_payload_t *pload = NULL;

  hand_ctx_t ctx;

  handlist = rk_list_create();

  /* setup search context */
  HandCtx_Clear(&ctx);

  /* build beat search context */
  Hand_CountRank(array, ctx.count);
  CardArray_Copy(&ctx.cards, array);

  /* extract bombs and 2 */
  HandList_ExtractNukeBomb2(handlist, &ctx.cards, ctx.count);

  /* finish building beat_search_context */
  CardArray_Copy(&ctx.rcards, &ctx.cards);
  CardArray_Reverse(&ctx.rcards);

  /* magic goes here */

  /* root */
  pload = (hltree_payload_t *)malloc(sizeof(hltree_payload_t));
  memcpy(&pload->ctx, &ctx, sizeof(hand_ctx_t));
  pload->weight = 0;
  grandtree = rk_tree_create(pload);

  /* first expansion */
  chains = rk_list_create();
  HLAA_ExtractAllChains(&ctx, chains);

  /* no chains, fall back to standard analyze */
  if (rk_list_empty(chains)) {
    rk_list_clear_destroy(handlist);
    rk_list_clear_destroy(chains);
    rk_tree_clear_destroy(grandtree);
    return Analysis_Standard(array);
  }

  /* got chains, make first expand */
  hlnode = chains->first;
  st = rk_list_create();

  while (hlnode != NULL) {
    tnode = HLAA_TreeAddHand(grandtree, hlnode);
    rk_list_push(st, tnode);

    hlnode = hlnode->next;
  }

  rk_list_clear_destroy(chains);

  /* loop start */
  while (!rk_list_empty(st)) {
    /* pop stack */
    workingtree = rk_list_pop(st);
    chains = rk_list_create();
    pload = (hltree_payload_t *)workingtree->payload;

    /* expansion */
    HLAA_ExtractAllChains(&pload->ctx, chains);

    if (!rk_list_empty(chains)) {
      /* push new nodes */
      hlnode = chains->first;

      while (hlnode != NULL) {
        tnode = HLAA_TreeAddHand(workingtree, hlnode);
        rk_list_push(st, tnode);

        hlnode = hlnode->next;
      }
    }

    rk_list_clear_destroy(chains);
  }

  /* tree construction complete */
  rk_tree_dump_leaves(grandtree, st);

  /* find shortest path */
  while (!rk_list_empty(st)) {
    /* pop stack */
    workingtree = rk_list_pop(st);
    pload = (hltree_payload_t *)workingtree->payload;

    /* calculate other hands weight */
    pload->weight += Analysis_CountHands(Analysis_Standard, &pload->ctx.cards);

    if ((shortest == NULL) ||
        (pload->weight < ((hltree_payload_t *)shortest->payload)->weight))
      shortest = workingtree;
  }

  rk_list_clear_destroy(st);

  /* extract shortest node's other hands */
  others =
      Analysis_Standard(&((hltree_payload_t *)(shortest->payload))->ctx.cards);

  while (shortest != NULL &&
         ((hltree_payload_t *)shortest->payload)->weight != 0) {
    HandList_PushFront(
        others, &((hltree_payload_t *)(shortest->payload))->hand);
    shortest = shortest->parent;
  }

  rk_list_concat(others, handlist);
  handlist->first = NULL;
  handlist->last = NULL;
  rk_list_destroy(handlist);

  rk_tree_clear_destroy(grandtree);

  return others;
}
