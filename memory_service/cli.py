"""CLI for exercising Hasth Vani + Hindsight memory."""

from __future__ import annotations

import argparse
import json

from hasth_vani_memory import HasthVaniMemory, HindsightError


def main() -> int:
    parser = argparse.ArgumentParser(description="Hasth Vani Hindsight memory CLI")
    sub = parser.add_subparsers(dest="command", required=True)

    retain = sub.add_parser("retain", help="Store an engineering observation")
    retain.add_argument("content")
    retain.add_argument("--context", default="Hasth Vani engineering observation")

    recall = sub.add_parser("recall", help="Recall engineering memory")
    recall.add_argument("query")
    recall.add_argument("--budget", default="mid")

    args = parser.parse_args()
    memory = HasthVaniMemory()

    try:
        if args.command == "retain":
            result = memory.retain(args.content, context=args.context)
        else:
            result = memory.recall(args.query, budget=args.budget)
    except HindsightError as exc:
        parser.exit(1, f"ERROR: {exc}\n")

    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
