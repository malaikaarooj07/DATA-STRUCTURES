#include <windows.h>
#include <string>
using namespace std;

// =====================
// Doubly Linked List
// =====================
struct Contact {
    wstring name;
    wstring phone;
    Contact* prev;
    Contact* next;
};

Contact* head = NULL;

Contact* createContact(const wstring& name, const wstring& phone) {
    Contact* c = new Contact();
    c->name = name;
    c->phone = phone;
    c->prev = c->next = NULL;
    return c;
}

void addContact(const wstring& name, const wstring& phone) {
    Contact* c = createContact(name, phone);
    if (!head) {
        head = c;
        return;
    }
    Contact* temp = head;
    while (temp->next)
        temp = temp->next;
    temp->next = c;
    c->prev = temp;
}

wstring displayContacts() {
    if (!head) return L"No contacts available.";

    wstring result = L"Phone Directory:\n\n";
    Contact* temp = head;
    while (temp) {
        result += L"Name: " + temp->name + L" | Phone: " + temp->phone + L"\n";
        temp = temp->next;
    }
    return result;
}

wstring searchContacts(const wstring& prefix) {
    Contact* temp = head;
    wstring result;

    while (temp) {
        if (temp->name.find(prefix) == 0) {
            result += temp->name + L" - " + temp->phone + L"\n";
        }
        temp = temp->next;
    }
    return result.empty() ? L"No match found." : result;
}

bool deleteContact(const wstring& name) {
    Contact* temp = head;
    while (temp) {
        if (temp->name == name) {
            if (temp->prev)
                temp->prev->next = temp->next;
            else
                head = temp->next;

            if (temp->next)
                temp->next->prev = temp->prev;

            delete temp;
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// =====================
// GUI Part
// =====================
HWND hName, hPhone;

#define ADD_BTN     1
#define SHOW_BTN    2
#define SEARCH_BTN  3
#define DELETE_BTN  4
#define EXIT_BTN    5

// Function to show a new result window
void showResultWindow(HINSTANCE hInst, const wstring& title, const wstring& message) {
    static bool isRegistered = false;

    if (!isRegistered) {
        WNDCLASSW wc = {};
        wc.lpszClassName = L"ResultWindow";
        wc.hInstance = hInst;
        wc.lpfnWndProc = DefWindowProc;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1);
        RegisterClassW(&wc);
        isRegistered = true;
    }

    HWND hwndResult = CreateWindowW(
        L"ResultWindow",
        title.c_str(),
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        450, 250, 350, 250,
        NULL, NULL, hInst, NULL);

    CreateWindowW(L"EDIT", message.c_str(),
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_HSCROLL,
        10, 10, 320, 200,
        hwndResult, NULL, hInst, NULL);

    ShowWindow(hwndResult, SW_SHOW);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    wchar_t name[100], phone[100];

    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Name:",
            WS_VISIBLE | WS_CHILD, 20, 20, 80, 20,
            hwnd, NULL, NULL, NULL);

        hName = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            100, 20, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"Phone:",
            WS_VISIBLE | WS_CHILD, 20, 60, 80, 20,
            hwnd, NULL, NULL, NULL);

        hPhone = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            100, 60, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Add",
            WS_VISIBLE | WS_CHILD,
            20, 110, 100, 30, hwnd, (HMENU)ADD_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Display",
            WS_VISIBLE | WS_CHILD,
            140, 110, 100, 30, hwnd, (HMENU)SHOW_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Search",
            WS_VISIBLE | WS_CHILD,
            20, 160, 100, 30, hwnd, (HMENU)SEARCH_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Delete",
            WS_VISIBLE | WS_CHILD,
            140, 160, 100, 30, hwnd, (HMENU)DELETE_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Exit",
            WS_VISIBLE | WS_CHILD,
            80, 210, 100, 30, hwnd, (HMENU)EXIT_BTN, NULL, NULL);
        break;

    case WM_COMMAND:
        GetWindowTextW(hName, name, 100);
        GetWindowTextW(hPhone, phone, 100);

        switch (LOWORD(wp)) {
        case ADD_BTN:
            if (wcslen(name) && wcslen(phone)) {
                addContact(name, phone);
                MessageBoxW(hwnd, L"Contact Added!", L"Success", MB_OK);
            } else {
                MessageBoxW(hwnd, L"Enter Name and Phone", L"Error", MB_OK);
            }
            break;

        case SHOW_BTN:
            MessageBoxW(hwnd, displayContacts().c_str(),
                L"Contacts", MB_OK);
            break;

        case SEARCH_BTN: {
            wstring result = searchContacts(name);
            showResultWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), L"Search Result", result);
            break;
        }

        case DELETE_BTN: {
            bool deleted = deleteContact(name);
            wstring msg = deleted ? L"Contact Deleted" : L"Contact Not Found";
            showResultWindow((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), L"Delete Result", msg);
            break;
        }

        case EXIT_BTN:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

// =====================
// WinMain
// =====================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmd) {
    WNDCLASSW wc = {};
    wc.lpszClassName = L"PhoneDirGUI";
    wc.hInstance = hInst;
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(
        wc.lpszClassName,
        L"Phone Directory (GUI)",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        300, 200, 350, 320,
        NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, nCmd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
