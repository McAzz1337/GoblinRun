using System.Collections;
using UnityEngine;

public class GameStarter : MonoBehaviour
{
    public GoblinSpawner spawner;
    public bool autoSpawnOnStart = false;

    IEnumerator Start()
    {
        // bisschen warten bis Meta XR ready ist
        yield return null;
        yield return new WaitForSeconds(0.5f);

        if (OVRManager.display != null)
        {
            OVRManager.display.RecenterPose();
        }

        yield return new WaitForSeconds(0.2f);

        if (autoSpawnOnStart && spawner != null)
        {
            // wenn du später doch eine Startwelle willst, nimm hier SpawnOneGoblin oder eigene Logik
            spawner.SpawnOneGoblin();
        }
        else
        {
            Debug.Log("[GameStarter] Autospawn deaktiviert, Goblins kommen nur über Web Commands");
        }
    }
}
