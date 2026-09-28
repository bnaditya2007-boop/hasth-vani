"""Small Hindsight adapter used by Hasth Vani's engineering memory layer.

The ESP32s should not hold Hindsight credentials or make cloud calls directly.
This adapter is intended to run on the laptop/Raspberry Pi/mobile gateway that
already has access to the glove telemetry and can safely hold the Hindsight
API key.
"""

from __future__ import annotations

import os
from dataclasses import dataclass
from typing import Any, Dict, List, Optional

import requests
from dotenv import load_dotenv

load_dotenv()


@dataclass(frozen=True)
class HindsightConfig:
    base_url: str = os.getenv("HINDSIGHT_BASE_URL", "http://localhost:8888")
    api_key: str = os.getenv("HINDSIGHT_API_KEY", "")
    bank_id: str = os.getenv("HINDSIGHT_BANK_ID", "hasth-vani")
    timeout_seconds: float = float(os.getenv("HINDSIGHT_TIMEOUT_SECONDS", "15"))


class HindsightError(RuntimeError):
    """Raised when Hindsight cannot complete a memory operation."""


class HasthVaniMemory:
    """Retain and recall project-specific engineering memory with Hindsight."""

    def __init__(self, config: Optional[HindsightConfig] = None) -> None:
        self.config = config or HindsightConfig()
        self.base_url = self.config.base_url.rstrip("/")

    def _headers(self) -> Dict[str, str]:
        headers = {"Content-Type": "application/json"}
        if self.config.api_key:
            headers["Authorization"] = f"Bearer {self.config.api_key}"
        return headers

    def _post(self, path: str, payload: Dict[str, Any]) -> Dict[str, Any]:
        url = f"{self.base_url}{path}"
        try:
            response = requests.post(
                url,
                json=payload,
                headers=self._headers(),
                timeout=self.config.timeout_seconds,
            )
        except requests.RequestException as exc:
            raise HindsightError(f"Hindsight request failed: {exc}") from exc

        if not response.ok:
            detail = response.text[:1000]
            raise HindsightError(
                f"Hindsight returned HTTP {response.status_code}: {detail}"
            )

        try:
            return response.json()
        except ValueError as exc:
            raise HindsightError("Hindsight returned non-JSON data") from exc

    def retain(
        self,
        content: str,
        *,
        context: str = "Hasth Vani engineering observation",
    ) -> Dict[str, Any]:
        """Persist an observation that should affect future debugging."""
        payload = {
            "items": [
                {
                    "content": content,
                    "context": context,
                }
            ]
        }
        # Retain is POST on the collection itself; there is no /retain suffix.
        return self._post(
            f"/v1/default/banks/{self.config.bank_id}/memories",
            payload,
        )

    def recall(
        self,
        query: str,
        *,
        budget: str = "mid",
    ) -> Dict[str, Any]:
        """Retrieve memories relevant to a debugging question."""
        payload = {"query": query, "budget": budget}
        return self._post(
            f"/v1/default/banks/{self.config.bank_id}/memories/recall",
            payload,
        )

    def remember_sensor_observation(
        self,
        sensor: str,
        reading: int,
        *,
        condition: str,
        conclusion: str,
        next_check: str,
    ) -> Dict[str, Any]:
        """Store a structured Hasth Vani sensor-debugging observation."""
        content = (
            f"Sensor: {sensor}. Reading: {reading}. Condition: {condition}. "
            f"Conclusion: {conclusion}. Next check: {next_check}."
        )
        return self.retain(content, context="Hasth Vani sensor debugging")
