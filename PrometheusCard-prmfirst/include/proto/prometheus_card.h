#ifndef _PROTO_PROMETHEUS_CARD_H
#define _PROTO_PROMETHEUS_CARD_H

/* Prometheus.card 8 for callers: proto + inline only, no clib header.
 * The base is PrometheusCardBase, so it does not clash with a card driver's
 * own CardBase. Open it as prometheus.library does:
 * OpenLibrary("Picasso96/Prometheus.card", 8). */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *PrometheusCardBase;
#endif

#if defined(__VBCC__)
#include <inline/prometheus_card_protos.h>
#elif defined(__GNUC__)
#include <inline/prometheus_card.h>
#endif

#endif /* _PROTO_PROMETHEUS_CARD_H */
