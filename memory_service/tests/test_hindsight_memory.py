from unittest.mock import Mock, patch

from hasth_vani_memory import HasthVaniMemory, HindsightConfig


def test_retain_posts_to_project_bank():
    config = HindsightConfig(
        base_url="http://hindsight.test",
        api_key="secret",
        bank_id="hasth-vani",
        timeout_seconds=5,
    )
    memory = HasthVaniMemory(config)

    response = Mock()
    response.ok = True
    response.json.return_value = {"ok": True}

    with patch("hasth_vani_memory.requests.post", return_value=response) as post:
        result = memory.retain("thumb=247", context="sensor test")

    assert result == {"ok": True}
    post.assert_called_once_with(
        "http://hindsight.test/v1/default/banks/hasth-vani/memories",
        json={"items": [{"content": "thumb=247", "context": "sensor test"}]},
        headers={
            "Content-Type": "application/json",
            "Authorization": "Bearer secret",
        },
        timeout=5,
    )


def test_recall_posts_query_and_budget():
    # api_key is set explicitly so a real .env on the machine can't leak into
    # the expected headers below.
    config = HindsightConfig(
        base_url="http://hindsight.test", api_key="", bank_id="hasth-vani"
    )
    memory = HasthVaniMemory(config)

    response = Mock()
    response.ok = True
    response.json.return_value = {"results": []}

    with patch("hasth_vani_memory.requests.post", return_value=response) as post:
        result = memory.recall("thumb flex sensor")

    assert result == {"results": []}
    post.assert_called_once_with(
        "http://hindsight.test/v1/default/banks/hasth-vani/memories/recall",
        json={"query": "thumb flex sensor", "budget": "mid"},
        headers={"Content-Type": "application/json"},
        timeout=15,
    )
