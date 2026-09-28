# Hasth Vani + Hindsight Memory

This directory adds the persistent engineering-memory layer to Hasth Vani.

## Why this runs off the ESP32

The ESP32 firmware should stay focused on sensor acquisition and real-time
hardware behavior. It should not contain a Hindsight API key or make cloud
requests from the glove.

The intended production path is:

```text
Flex / MPU6050 / glove firmware
              |
              v
       Gateway / app layer
              |
              v
      Hasth Vani Memory Adapter
              |
              v
           Hindsight
```

The gateway can be a laptop, Raspberry Pi, phone backend, or another trusted
service that already receives glove telemetry. This keeps credentials outside
the firmware and makes memory available to the debugging/agent layer.

## Setup

1. Copy `.env.example` to `.env`.
2. Set `HINDSIGHT_BASE_URL` to your Hindsight deployment.
3. Set `HINDSIGHT_API_KEY` if the deployment requires authentication.
4. Keep `HINDSIGHT_BANK_ID=hasth-vani` so this project's memories stay isolated.
5. Install dependencies:

```bash
python -m pip install -r requirements.txt
```

## Retain an observation

```bash
python cli.py retain "Thumb channel measured about 247 after direct 3.3V bypass; sensor itself is not yet proven to be the cause. Next check: verify the divider resistor and swap the sensor." --context "Hasth Vani flex-sensor debugging"
```

## Recall it later

```bash
python cli.py recall "What do we already know about the thumb flex sensor?"
```

The recall result should be used by the agent/debugging workflow before it
chooses the next experiment.

## Structured sensor helper

```python
from hasth_vani_memory import HasthVaniMemory

memory = HasthVaniMemory()
memory.remember_sensor_observation(
    sensor="thumb",
    reading=247,
    condition="straight, direct 3.3V bypass",
    conclusion="the flex sensor alone is not proven to cause the low reading",
    next_check="verify the resistor value and swap the sensor",
)
```

## Important

This adapter uses Hindsight's HTTP retain/recall API. Test it against the
specific Hindsight deployment you use before production rollout, because
server authentication and deployment settings can differ between hosted and
self-hosted environments.

## Test the adapter without contacting Hindsight

From `memory_service/`:

```bash
python -m pip install -r requirements-dev.txt
python -m pytest -q tests
```

These tests mock the HTTP transport and verify the retain/recall requests,
including the project bank and authentication header.
