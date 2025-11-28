using UnityEngine;

public class TrapManager : MonoBehaviour
{
    public Trap[] traps;

    public void TriggerTrap(string trapId)
    {
        if (string.IsNullOrEmpty(trapId))
        {
            Debug.LogWarning("TrapManager TriggerTrap ohne trapId");
            return;
        }

        string idUpper = trapId.ToUpperInvariant();

        foreach (var trap in traps)
        {
            if (trap == null || string.IsNullOrEmpty(trap.trapId))
                continue;

            if (trap.trapId.ToUpperInvariant() == idUpper)
            {
                trap.Activate();
                return;
            }
        }

        Debug.LogWarning("TrapManager: keine Trap mit Id " + trapId + " gefunden");
    }
}
