/**
 * =========================================================
 * @author      Anas Majdi 
 * @brief       Idea Owner      : Lujain Mohammad
 * @note        Team Members    : Anas & Lujain
 * @attention   Supervisor      : Dr. Ammar Issa
 * =========================================================
 */

#undef UNICODE
#undef _UNICODE

#include <windows.h>
#include <string>
#include <stdexcept>
#include "MatrixMath.h"

// 1. Grouping Global UI Elements into a single struct (Cleaner Global Namespace)
struct UIElements {
    HWND btnSystems, btnOps, btnBack; 
    HWND editA, editB, lblResult;      
    HWND btnDet, btnInv, btnTrans, btnTrace, btnSym;
    HWND btnAdd, btnSub, btnMul, btnCramer, btnGauss;
    HWND lblA, lblB;
    
    HBRUSH bgBrush;       
    HBRUSH darkGrayBrush; 
    HFONT customFont;
} UI;

enum AppMode { MODE_MAIN, MODE_SYSTEMS, MODE_OPS };
AppMode currentMode = MODE_MAIN;

// 2. UI Layout Management
void ResizeLayout(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    if (currentMode == MODE_MAIN) {
        int btnW = 230, btnH = 45;
        int startX = (cx - btnW) / 2;
        MoveWindow(UI.btnSystems, startX, cy / 2 - 40, btnW, btnH, TRUE);
        MoveWindow(UI.btnOps, startX, cy / 2 + 10, btnW, btnH, TRUE);
    } 
    else if (currentMode == MODE_SYSTEMS || currentMode == MODE_OPS) {
        MoveWindow(UI.btnBack, 20, 15, 120, 30, TRUE);

        int boxW = 230, boxH = 100, gap = 20;
        int totalW = (boxW * 2) + gap;
        int startX = (cx - totalW) / 2;

        MoveWindow(UI.lblA, startX, 55, boxW, 20, TRUE);
        MoveWindow(UI.editA, startX, 80, boxW, boxH, TRUE);

        MoveWindow(UI.lblB, startX + boxW + gap, 55, boxW, 20, TRUE);
        MoveWindow(UI.editB, startX + boxW + gap, 80, boxW, boxH, TRUE);

        MoveWindow(UI.lblResult, startX, 290, totalW, cy - 310, TRUE);

        if (currentMode == MODE_OPS) {
            int bW = 88, bH = 35, bGap = 6;
            int r1TotalW = (bW * 5) + (bGap * 4);
            int r1X = (cx - r1TotalW) / 2;
            MoveWindow(UI.btnDet,   r1X, 195, bW, bH, TRUE);
            MoveWindow(UI.btnInv,   r1X + (bW + bGap), 195, bW, bH, TRUE);
            MoveWindow(UI.btnTrans, r1X + (bW + bGap)*2, 195, bW, bH, TRUE);
            MoveWindow(UI.btnTrace, r1X + (bW + bGap)*3, 195, bW, bH, TRUE);
            MoveWindow(UI.btnSym,   r1X + (bW + bGap)*4, 195, bW, bH, TRUE);

            int r2W = 140, r2Gap = 15;
            int r2TotalW = (r2W * 3) + (r2Gap * 2);
            int r2X = (cx - r2TotalW) / 2;
            MoveWindow(UI.btnAdd, r2X, 240, r2W, bH, TRUE);
            MoveWindow(UI.btnSub, r2X + (r2W + r2Gap), 240, r2W, bH, TRUE);
            MoveWindow(UI.btnMul, r2X + (r2W + r2Gap)*2, 240, r2W, bH, TRUE);
        } 
        else if (currentMode == MODE_SYSTEMS) {
            int sW = 180, sH = 35, sGap = 20;
            int sysTotalW = (sW * 2) + sGap;
            int sysX = (cx - sysTotalW) / 2;
            MoveWindow(UI.btnCramer, sysX, 195, sW, sH, TRUE);
            MoveWindow(UI.btnGauss, sysX + sW + sGap, 195, sW, sH, TRUE);
        }
    }
}

// 3. UI State Management
void SwitchMode(AppMode mode, HWND hwnd) {
    currentMode = mode;
    ShowWindow(UI.btnSystems, (mode == MODE_MAIN) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnOps, (mode == MODE_MAIN) ? SW_SHOW : SW_HIDE);
    
    bool isVisible = (mode != MODE_MAIN);
    ShowWindow(UI.btnBack, isVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.editA, isVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.editB, isVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.lblA, isVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.lblB, isVisible ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.lblResult, isVisible ? SW_SHOW : SW_HIDE);

    ShowWindow(UI.btnDet, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnInv, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnTrans, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnTrace, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnSym, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnAdd, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnSub, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnMul, (mode == MODE_OPS) ? SW_SHOW : SW_HIDE);
    
    ShowWindow(UI.btnCramer, (mode == MODE_SYSTEMS) ? SW_SHOW : SW_HIDE);
    ShowWindow(UI.btnGauss, (mode == MODE_SYSTEMS) ? SW_SHOW : SW_HIDE);
    
    ResizeLayout(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
}

// 4. Separation of Concerns: Handling Math Logic Outside WndProc
void ExecuteMathCommand(int cmd, HWND hwnd) {
    char bufA[2048], bufB[2048]; 
    GetWindowText(UI.editA, bufA, 2048); 
    GetWindowText(UI.editB, bufB, 2048);
    
    try {
        // Delegate parsing to the LinearAlgebra namespace
        LinearAlgebra::Matrix A = LinearAlgebra::ParseMatrix(bufA); 
        if (A.empty()) throw std::runtime_error("Matrix A is empty!");
        
        std::string res = "";

        if (cmd >= 6 && cmd <= 8) { 
            if (std::string(bufB).empty()) throw std::runtime_error("Matrix B required!");
            LinearAlgebra::Matrix B_mat = LinearAlgebra::ParseMatrix(bufB);
            
            if(cmd == 6) res = "A + B:\n" + LinearAlgebra::MatrixToString(LinearAlgebra::AddMatrices(A, B_mat));
            else if(cmd == 7) res = "A - B:\n" + LinearAlgebra::MatrixToString(LinearAlgebra::SubtractMatrices(A, B_mat));
            else if(cmd == 8) res = "A * B:\n" + LinearAlgebra::MatrixToString(LinearAlgebra::MultiplyMatrices(A, B_mat));
        } 
        else if (cmd >= 9 && cmd <= 10) { 
            if (std::string(bufB).empty()) throw std::runtime_error("Vector B required!");
            std::vector<double> B_vec = LinearAlgebra::ParseVector(bufB);
            
            if(cmd == 9) res = "Cramer Rule Solution:\n" + LinearAlgebra::VectorToString(LinearAlgebra::CramerRule(A, B_vec));
            else if(cmd == 10) res = "Gauss-Jordan Solution:\n" + LinearAlgebra::VectorToString(LinearAlgebra::GaussJordan(A, B_vec));
        } 
        else { 
            if(cmd == 1) res = "Determinant = " + std::to_string(LinearAlgebra::Determinant(A));
            else if(cmd == 2) res = "Inverse Matrix:\n" + LinearAlgebra::MatrixToString(LinearAlgebra::Inverse(A));
            else if(cmd == 3) res = "Transposed Matrix:\n" + LinearAlgebra::MatrixToString(LinearAlgebra::Transpose(A));
            else if(cmd == 4) res = "Trace = " + std::to_string(LinearAlgebra::Trace(A));
            else if(cmd == 5) {
                bool sym = LinearAlgebra::IsSymmetric(A);
                res = sym ? "Result: Matrix A is SYMMETRIC (True)" : "Result: Matrix A is NOT Symmetric (False)";
            }
        }
        SetWindowText(UI.lblResult, res.c_str());
    } 
    catch (const std::exception& e) { 
        MessageBox(hwnd, e.what(), "Error", MB_ICONERROR); 
    }
}

// 5. Main Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            UI.bgBrush = CreateSolidBrush(RGB(10, 14, 23));       
            UI.darkGrayBrush = CreateSolidBrush(RGB(20, 26, 40)); 
            UI.customFont = CreateFont(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, 
                                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                     CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

            auto CreateCustomButton = [&](const char* text, int x, int y, int w, int h, HMENU id) {
                HWND btn = CreateWindow("BUTTON", text, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x, y, w, h, hwnd, id, NULL, NULL);
                SendMessage(btn, WM_SETFONT, (WPARAM)UI.customFont, TRUE);
                return btn;
            };

            UI.btnSystems = CreateCustomButton("Solve Linear Systems", 0, 0, 230, 45, (HMENU)100);
            UI.btnOps     = CreateCustomButton("Matrix Operations", 0, 0, 230, 45, (HMENU)101);
            UI.btnBack    = CreateCustomButton("< Back to Menu", 0, 0, 120, 30, (HMENU)102);
            
            UI.lblA = CreateWindow("STATIC", "Matrix A (or Coeffs):", WS_CHILD, 0, 0, 230, 20, hwnd, NULL, NULL, NULL);
            UI.lblB = CreateWindow("STATIC", "Matrix B (or Vector):", WS_CHILD, 0, 0, 230, 20, hwnd, NULL, NULL, NULL);
            UI.editA = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 0, 0, 230, 100, hwnd, NULL, NULL, NULL);
            UI.editB = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 0, 0, 230, 100, hwnd, NULL, NULL, NULL);
            
            UI.btnDet   = CreateCustomButton("Det (A)", 0, 0, 88, 35, (HMENU)1);
            UI.btnInv   = CreateCustomButton("Inverse (A)", 0, 0, 88, 35, (HMENU)2);
            UI.btnTrans = CreateCustomButton("Trans (A)", 0, 0, 88, 35, (HMENU)3);
            UI.btnTrace = CreateCustomButton("Trace (A)", 0, 0, 88, 35, (HMENU)4);
            UI.btnSym   = CreateCustomButton("Symmetric", 0, 0, 88, 35, (HMENU)5); 
            
            UI.btnAdd = CreateCustomButton("A + B", 0, 0, 140, 35, (HMENU)6);
            UI.btnSub = CreateCustomButton("A - B", 0, 0, 140, 35, (HMENU)7);
            UI.btnMul = CreateCustomButton("A * B", 0, 0, 140, 35, (HMENU)8);
            
            UI.btnCramer = CreateCustomButton("Cramer Rule", 0, 0, 180, 35, (HMENU)9);
            UI.btnGauss  = CreateCustomButton("Gauss-Jordan", 0, 0, 180, 35, (HMENU)10);
            
            UI.lblResult = CreateWindow("STATIC", "Result will appear here...", WS_CHILD, 0, 0, 460, 190, hwnd, NULL, NULL, NULL);

            SendMessage(UI.lblA, WM_SETFONT, (WPARAM)UI.customFont, TRUE);
            SendMessage(UI.lblB, WM_SETFONT, (WPARAM)UI.customFont, TRUE);
            SendMessage(UI.editA, WM_SETFONT, (WPARAM)UI.customFont, TRUE);
            SendMessage(UI.editB, WM_SETFONT, (WPARAM)UI.customFont, TRUE);
            SendMessage(UI.lblResult, WM_SETFONT, (WPARAM)UI.customFont, TRUE);

            SwitchMode(MODE_MAIN, hwnd);
            break;
        }

        case WM_SIZE:
            ResizeLayout(hwnd);
            break;

        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
            if (pdis->CtlType == ODT_BUTTON) {
                HBRUSH hBr = CreateSolidBrush((pdis->itemState & ODS_SELECTED) ? RGB(218, 41, 28) : RGB(20, 26, 40));
                FillRect(pdis->hDC, &pdis->rcItem, hBr);
                DeleteObject(hBr);

                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(218, 41, 28));
                HPEN hOldPen = (HPEN)SelectObject(pdis->hDC, hPen);
                HBRUSH hOldBr = (HBRUSH)SelectObject(pdis->hDC, GetStockObject(NULL_BRUSH));
                Rectangle(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right, pdis->rcItem.bottom);
                SelectObject(pdis->hDC, hOldPen);
                SelectObject(pdis->hDC, hOldBr);
                DeleteObject(hPen);

                char text[256];
                GetWindowText(pdis->hwndItem, text, sizeof(text));
                SetTextColor(pdis->hDC, (pdis->itemState & ODS_SELECTED) ? RGB(255, 255, 255) : RGB(218, 41, 28));
                SetBkMode(pdis->hDC, TRANSPARENT);
                SelectObject(pdis->hDC, UI.customFont);
                DrawText(pdis->hDC, text, -1, &pdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            return TRUE;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255)); 
            SetBkColor(hdcStatic, RGB(10, 14, 23));          
            return (INT_PTR)UI.bgBrush;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            SetTextColor(hdcEdit, RGB(255, 255, 255));    
            SetBkColor(hdcEdit, RGB(20, 26, 40));         
            return (INT_PTR)UI.darkGrayBrush;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == 100) SwitchMode(MODE_SYSTEMS, hwnd);
            else if (LOWORD(wParam) == 101) SwitchMode(MODE_OPS, hwnd);
            else if (LOWORD(wParam) == 102) SwitchMode(MODE_MAIN, hwnd);
            else if (LOWORD(wParam) >= 1 && LOWORD(wParam) <= 10) {
                // Call the isolated logic function
                ExecuteMathCommand(LOWORD(wParam), hwnd);
            }
            break;
            
        case WM_DESTROY: 
            DeleteObject(UI.bgBrush);
            DeleteObject(UI.darkGrayBrush);
            DeleteObject(UI.customFont);
            PostQuitMessage(0); 
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0}; 
    wc.lpfnWndProc = WndProc; 
    wc.hInstance = hInstance; 
    wc.lpszClassName = "LinearAlgebraApp"; 
    wc.hbrBackground = CreateSolidBrush(RGB(10, 14, 23));
    
    RegisterClass(&wc);
    HWND hwnd = CreateWindow("LinearAlgebraApp", "Anas Majdi - Matrix Calculator", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 520, 530, NULL, NULL, hInstance, NULL);
    MSG msg = {0}; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}

/*
=========================================================
* Compilation and Execution Commands:
* (Copy and paste into terminal)
* 
 g++ main.cpp MatrixMath.cpp -o MatrixApp -lgdi32
 .\MatrixApp
=========================================================
*/