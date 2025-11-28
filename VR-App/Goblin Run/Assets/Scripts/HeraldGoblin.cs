using System.Collections;
using UnityEngine;

public class HeraldGoblin : MonoBehaviour
{
    [Header("Behaviour")]
    public float lookAtPlayerDuration = 1.0f;
    public float runSpeed = 2.5f;
    public float runDistance = 5f;

    [Header("Follow-up Wave")]
    public int followupGoblinCount = 3;
    public float followupDelay = 1.5f;

    public AudioSource screamAudio;

    private Transform playerTarget;
    private GoblinSpawner spawner;
    private Vector3 runDirection;
    private bool isRunning = false;
    private Vector3 startPos;

    public void Init(Transform player, GoblinSpawner spawnerRef, Vector3 runDir)
    {
        playerTarget = player;
        spawner = spawnerRef;
        runDirection = runDir.normalized;
    }

    void Start()
    {
        startPos = transform.position;
        StartCoroutine(BehaviourRoutine());
    }

    IEnumerator BehaviourRoutine()
    {
        // 1. Spieler anschauen
        float t = 0f;
        if (playerTarget != null)
        {
            while (t < lookAtPlayerDuration)
            {
                t += Time.deltaTime;

                Vector3 dir = playerTarget.position - transform.position;
                dir.y = 0f;
                if (dir.sqrMagnitude > 0.001f)
                {
                    Quaternion targetRot = Quaternion.LookRotation(dir.normalized, Vector3.up);
                    transform.rotation = Quaternion.Slerp(transform.rotation, targetRot, Time.deltaTime * 5f);
                }
                yield return null;
            }
        }
        else
        {
            yield return new WaitForSeconds(lookAtPlayerDuration);
        }

        if (screamAudio != null)
            screamAudio.Play();

        // 2. Umdrehen und wegrennen
        if (runDirection.sqrMagnitude > 0.001f)
        {
            Quaternion awayRot = Quaternion.LookRotation(runDirection, Vector3.up);
            transform.rotation = awayRot;
        }

        isRunning = true;
        Vector3 targetPos = startPos + runDirection * runDistance;
        float distRun = 0f;

        while (distRun < runDistance)
        {
            float step = runSpeed * Time.deltaTime;
            transform.position += runDirection * step;
            distRun += step;
            yield return null;
        }

        isRunning = false;

        // **NEUER TEIL: warten bevor Wave kommt**
        yield return new WaitForSeconds(followupDelay);

        // 3. Follow-Up Goblins spawnen
        if (spawner != null && followupGoblinCount > 0)
        {
            spawner.SpawnHeraldFollowupWave(followupGoblinCount);
        }

        Destroy(gameObject, 1.0f);
    }
}
