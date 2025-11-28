using UnityEngine;

public class GoblinSpawner : MonoBehaviour
{
    [Header("References")]
    public Transform playerRig;
    public Transform playerHead;

    [Tooltip("Prefab des normalen Goblins.")]
    public GameObject goblinPrefab;

    [Tooltip("Prefab des Herald Goblins.")]
    public GameObject heraldPrefab;

    [Header("Spawn Settings")]
    public int numberOfGoblins = 3;
    public float minDistance = 2f;
    public float maxDistance = 5f;
    public float groundHeightOffset = 0f;

    [Header("Herald Wave Settings")]
    public int heraldWaveCount = 3;
    public float heraldWaveMinDistance = 7f;
    public float heraldWaveMaxDistance = 12f;

    // NEU: gemeinsame Setup-Methode
    void SetupGoblin(GameObject goblin, bool forceChase)
    {
        if (goblin == null) return;

        Transform chaseTarget = playerHead != null ? playerHead : playerRig;

        GoblinChase chase = goblin.GetComponent<GoblinChase>();
        if (chase != null && chaseTarget != null)
        {
            chase.SetTarget(chaseTarget);
            if (forceChase)
            {
                chase.ForceChase();
            }
        }

        GoblinHealth hp = goblin.GetComponent<GoblinHealth>();
        if (hp != null && chaseTarget != null)
        {
            hp.SetPlayerTarget(chaseTarget);
        }
    }


    // -------------------------------------------------------------
    //        KOMMANDO-API
    // -------------------------------------------------------------

    public void SpawnFromCommand(string enemyType, string position)
    {
        Debug.Log($"SpawnFromCommand enemyType={enemyType}, position={position}");

        if (string.IsNullOrEmpty(enemyType))
        {
            enemyType = "goblin";
        }

        string e = enemyType.ToLowerInvariant();

        float angleDeg = PositionToAngle(position);

        if (e == "goblin")
        {
            SpawnGoblinAtAngle(angleDeg);
            return;
        }

        if (e == "herald")
        {
            SpawnHerald(angleDeg);
            return;
        }

        Debug.LogWarning($"Unbekannter enemyType {enemyType}");
    }

    float PositionToAngle(string position)
    {
        if (string.IsNullOrEmpty(position)) return 0f;

        switch (position.ToUpperInvariant())
        {
            case "FRONT": return 0f;
            case "LEFT": return -60f;
            case "RIGHT": return 60f;
            case "BACK": return 180f;
            default:
                Debug.LogWarning($"Unbekannte position {position}. Nutze FRONT.");
                return 0f;
        }
    }


    // -------------------------------------------------------------
    //       NORMALE GOBLINS
    // -------------------------------------------------------------

    public void SpawnOneGoblin()
    {
        float angleDeg = Random.Range(-40f, 40f);
        SpawnGoblinAtAngle(angleDeg);
    }

    void SpawnGoblinAtAngle(float angleDeg)
    {
        SpawnGoblinAtAngleWithDistance(angleDeg, minDistance, maxDistance);
    }

    void SpawnGoblinAtAngleWithDistance(float angleDeg, float minDist, float maxDist)
    {
        if (playerRig == null || goblinPrefab == null)
        {
            Debug.LogWarning("Spawner nicht korrekt gesetzt.");
            return;
        }

        Vector3 origin = playerRig.position;
        origin.y = 0f;

        Vector3 fwd = playerRig.forward;
        fwd.y = 0f;
        fwd.Normalize();

        Vector3 dir = Quaternion.Euler(0f, angleDeg, 0f) * fwd;
        float dist = Random.Range(minDist, maxDist);

        Vector3 spawnPos = origin + dir * dist;
        spawnPos.y = groundHeightOffset;

        Quaternion lookRot = Quaternion.LookRotation((origin - spawnPos).normalized, Vector3.up);

        GameObject goblin = Instantiate(goblinPrefab, spawnPos, lookRot);

        // normale Goblins → KEIN ForceChase
        SetupGoblin(goblin, false);

        Debug.Log($"Normaler Goblin angle {angleDeg} dist {dist} pos {spawnPos}");
    }


    // -------------------------------------------------------------
    //       HERALD
    // -------------------------------------------------------------

    void SpawnHerald(float angleDeg)
    {
        if (playerRig == null || heraldPrefab == null)
        {
            Debug.LogWarning("Herald-Spawner nicht korrekt gesetzt.");
            return;
        }

        Vector3 origin = playerRig.position;
        origin.y = 0f;

        Vector3 fwd = playerRig.forward;
        fwd.y = 0f;
        fwd.Normalize();

        Vector3 dir = Quaternion.Euler(0f, angleDeg, 0f) * fwd;

        float dist = Mathf.Lerp(minDistance, maxDistance, 0.5f);
        Vector3 spawnPos = origin + dir * dist;
        spawnPos.y = groundHeightOffset;

        Quaternion lookRot = Quaternion.LookRotation((origin - spawnPos).normalized, Vector3.up);

        GameObject heraldObj = Instantiate(heraldPrefab, spawnPos, lookRot);

        Transform target = playerHead != null ? playerHead : playerRig;

        Vector3 runDir = (spawnPos - origin);
        runDir.y = 0f;
        runDir = runDir.sqrMagnitude > 0.01f ? runDir.normalized : dir;

        HeraldGoblin herald = heraldObj.GetComponent<HeraldGoblin>();
        if (herald != null)
        {
            herald.Init(target, this, runDir);
        }

        Debug.Log($"Herald gespawnt angle {angleDeg} pos {spawnPos}");
    }


    // -------------------------------------------------------------
    //       HERALD-FOLLOWUP WAVE (FORCE CHASE)
    // -------------------------------------------------------------

    public void SpawnHeraldFollowupWave(int count)
    {
        Debug.Log($"SpawnHeraldFollowupWave count={count}");

        float minDist = heraldWaveMinDistance > 0 ? heraldWaveMinDistance : maxDistance + 2f;
        float maxDist = heraldWaveMaxDistance > minDist ? heraldWaveMaxDistance : minDist + 3f;

        if (playerRig == null || goblinPrefab == null) return;

        Vector3 origin = playerRig.position;
        origin.y = 0f;

        for (int i = 0; i < count; i++)
        {
            float angleDeg = (360f / count) * i;

            Vector3 fwd = playerRig.forward;
            fwd.y = 0f;
            fwd.Normalize();

            Vector3 dir = Quaternion.Euler(0f, angleDeg, 0f) * fwd;
            float dist = Random.Range(minDist, maxDist);

            Vector3 spawnPos = origin + dir * dist;
            spawnPos.y = groundHeightOffset;

            Quaternion lookRot = Quaternion.LookRotation((origin - spawnPos).normalized, Vector3.up);

            GameObject goblin = Instantiate(goblinPrefab, spawnPos, lookRot);

            // **Herald Wave = FORCE CHASE**
            SetupGoblin(goblin, true);

            Debug.Log($"HeraldWave Goblin angle {angleDeg} dist {dist}");
        }
    }
}
