using UnityEngine;

public class ScreenBlocker : MonoBehaviour
{
    [SerializeField] private GameObject blackoutPanel;
    [SerializeField] private MonoBehaviour locomotionController;

    public bool IsBlocked { get; private set; }

    public void SetBlocked(bool blocked)
    {
        IsBlocked = blocked;

        if (blackoutPanel != null)
        {
            blackoutPanel.SetActive(blocked);
        }

        if (locomotionController != null)
        {
            locomotionController.enabled = !blocked;
        }

        Debug.Log("[ScreenBlocker] Blocked " + blocked);
    }
}
