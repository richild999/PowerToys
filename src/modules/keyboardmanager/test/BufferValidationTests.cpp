#include "pch.h"
#include "CppUnitTest.h"
#include <keyboardmanager/ui/BufferValidationHelpers.h>
#include "TestHelpers.h"
#include <common/keyboard_layout.h>
#include <common/shared_constants.h>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TestHelpers;

namespace RemappingUITests
{
    // Tests for methods in the BufferValidationHelpers namespace
    TEST_CLASS (BufferValidationTests)
    {
        std::wstring testApp1 = L"testprocess1.exe";
        std::wstring testApp2 = L"testprocess2.exe";
        LayoutMap keyboardLayout;

        struct ValidateAndUpdateKeyBufferElementArgs
        {
            int elementRowIndex;
            int elementColIndex;
            int selectedIndexFromDropDown;
        };

        struct ValidateShortcutBufferElementArgs
        {
            int elementRowIndex;
            int elementColIndex;
            uint32_t indexOfDropDownLastModified;
            std::vector<int32_t> selectedIndicesOnDropDowns;
            std::wstring targetAppNameInTextBox;
            bool isHybridColumn;
            RemapBufferRow bufferRow;
        };

        void RunTestCases(const std::vector<ValidateShortcutBufferElementArgs>& testCases, std::function<void(const ValidateShortcutBufferElementArgs&)> testMethod)
        {
            for (int i = 0; i < testCases.size(); i++)
            {
                testMethod(testCases[i]);
            }
        }

    public:
        TEST_METHOD_INITIALIZE(InitializeTestEnv)
        {
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to null in a new row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNullInANewRow)
        {
            RemapBuffer remapBuffer;

            // Add 2 empty rows
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, NULL }), std::wstring()));
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, NULL }), std::wstring()));

            // Validate and update the element when -1 i.e. null selection is made on an empty row.
            ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, -1 };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[1]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a new row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInANewRow)
        {
            RemapBuffer remapBuffer;

            // Add an empty row
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, NULL }), std::wstring()));

            // Validate and update the element when selecting B on an empty row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, GetDropDownIndexFromDropDownList(0x42, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to key
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToKeyRow)
        {
            RemapBuffer remapBuffer;

            // Add a row with A as the target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, 0x41 }), std::wstring()));

            // Validate and update the element when selecting B on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, GetDropDownIndexFromDropDownList(0x42, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)0x41, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to shortcut
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToShortcutRow)
        {
            RemapBuffer remapBuffer;

            // Add a row with Ctrl+A as the target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) }), std::wstring()));

            // Validate and update the element when selecting B on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, GetDropDownIndexFromDropDownList(0x42, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is validated and buffer is updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::NoError);
            Assert::AreEqual((DWORD)0x42, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual(true, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) == std::get<Shortcut>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column to the same value as the right column
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnMapToSameKeyError_OnSettingFirstColumnToSameValueAsRightColumn)
        {
            RemapBuffer remapBuffer;

            // Add a row with A as the target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, 0x41 }), std::wstring()));

            // Validate and update the element when selecting A on a row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, GetDropDownIndexFromDropDownList(0x41, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::MapToSameKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[0].first[0]));
            Assert::AreEqual((DWORD)0x41, std::get<DWORD>(remapBuffer[0].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to the same value as in another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToKeyRowToSameValueAsInAnotherRow)
        {
            RemapBuffer remapBuffer;

            // Add a row from A->B and a row with C as target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ 0x41, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, 0x43 }), std::wstring()));

            // Validate and update the element when selecting A on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, GetDropDownIndexFromDropDownList(0x41, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::SameKeyPreviouslyMapped);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)0x43, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to the same value as in another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToShortcutRowToSameValueAsInAnotherRow)
        {
            RemapBuffer remapBuffer;

            // Add a row from A->B and a row with Ctrl+A as target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ 0x41, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) }), std::wstring()));

            // Validate and update the element when selecting A on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, GetDropDownIndexFromDropDownList(0x41, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::SameKeyPreviouslyMapped);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual(true, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) == std::get<Shortcut>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to a conflicting modifier with another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToKeyRowToConflictingModifierWithAnotherRow)
        {
            RemapBuffer remapBuffer;

            // Add a row from Ctrl->B and a row with C as target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ VK_CONTROL, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, 0x43 }), std::wstring()));

            // Validate and update the element when selecting LCtrl on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual((DWORD)0x43, std::get<DWORD>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to a conflicting modifier with another row
        TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToShortcutRowToConflictingModifierWithAnotherRow)
        {
            RemapBuffer remapBuffer;

            // Add a row from Ctrl->B and a row with Ctrl+A as target
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ VK_CONTROL, 0x42 }), std::wstring()));
            remapBuffer.push_back(std::make_pair(RemapBufferItem({ NULL, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) }), std::wstring()));

            // Validate and update the element when selecting LCtrl on second row
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(false);
            ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList) };
            KeyboardManagerHelper::ErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedIndexFromDropDown, keyboardLayout.GetKeyCodeList(false), remapBuffer);

            // Assert that the element is invalid and buffer is not updated
            Assert::AreEqual(true, error == KeyboardManagerHelper::ErrorType::ConflictingModifierKey);
            Assert::AreEqual((DWORD)NULL, std::get<DWORD>(remapBuffer[1].first[0]));
            Assert::AreEqual(true, Shortcut(std::vector<DWORD>{ VK_CONTROL, 0x41 }) == std::get<Shortcut>(remapBuffer[1].first[1]));
        }

        // Test if the ValidateShortcutBufferElement method is successful and no drop down action is required on setting a column to null in a new or valid row
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingColumnToNullInANewOrValidRow)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when making null-selection (-1 index) on first column of empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when making null-selection (-1 index) on second column of empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when making null-selection (-1 index) on first column of empty shortcut to key row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), NULL }, std::wstring()) });
            // Case 4: Validate the element when making null-selection (-1 index) on second column of empty shortcut to key row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), NULL }, std::wstring()) });
            // Case 5: Validate the element when making null-selection (-1 index) on first dropdown of first column of valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(0x43, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 6: Validate the element when making null-selection (-1 index) on first dropdown of second column of valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(0x41, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 7: Validate the element when making null-selection (-1 index) on first dropdown of second column of valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(0x41, keyList) }), std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 8: Validate the element when making null-selection (-1 index) on second dropdown of first column of valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1 }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 9: Validate the element when making null-selection (-1 index) on second dropdown of second column of valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1 }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 10: Validate the element when making null-selection (-1 index) on second dropdown of second column of valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1 }), std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 11: Validate the element when making null-selection (-1 index) on first dropdown of first column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x44, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 12: Validate the element when making null-selection (-1 index) on first dropdown of second column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 13: Validate the element when making null-selection (-1 index) on first dropdown of second column of valid hybrid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>({ -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }), std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 14: Validate the element when making null-selection (-1 index) on second dropdown of first column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x44, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 15: Validate the element when making null-selection (-1 index) on second dropdown of second column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x42, keyList) }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 16: Validate the element when making null-selection (-1 index) on second dropdown of second column of valid hybrid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x42, keyList) }), std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 17: Validate the element when making null-selection (-1 index) on third dropdown of first column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), -1 }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 18: Validate the element when making null-selection (-1 index) on third dropdown of second column of valid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), -1 }), std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 19: Validate the element when making null-selection (-1 index) on third dropdown of second column of valid hybrid 3 key shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>({ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), -1 }), std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x44 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no drop down action is required on setting first drop down to an action key on a non-hybrid control column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnANonHybridColumn)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on first dropdown of first column of empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on first dropdown of second column of empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting A (0x41) on first dropdown of first column of empty shortcut to key row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), NULL }, std::wstring()) });
            // Case 4: Validate the element when selecting A (0x41) on first dropdown of first column of valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting first drop down to an action key on an empty hybrid control column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnAnEmptyHybridColumn)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on first dropdown of second column of empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on first dropdown of second column of empty shortcut to key row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), NULL }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting first drop down to an action key on a hybrid control column with full shortcut
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonLastDropDownToActionKeyOnAHybridColumnWithFullShortcut)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on first dropdown of second column of hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on second dropdown of second column of hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting non first non last drop down to an action key on a non hybrid control column with full shortcut
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonFirstNonLastDropDownToActionKeyOnANonHybridColumnWithFullShortcut)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on second dropdown of first column of shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting A  (0x41)on second dropdown of second column of shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutNotMoreThanOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting last drop down to an action key on a column with atleast two drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingLastDropDownToActionKeyOnAColumnWithAtleastTwoDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on last dropdown of first column of three key shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on last dropdown of second column of three key shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting A (0x41) on last dropdown of hybrid second column of three key shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting A (0x41) on last dropdown of first column of two key shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 5: Validate the element when selecting A (0x41) on last dropdown of second column of two key shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 6: Validate the element when selecting A (0x41) on last dropdown of hybrid second column of two key shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and no drop down action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting non first drop down to an action key on a column if all the drop downs after it are empty
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingNonFirstDropDownToActionKeyOnAColumnIfAllTheDropDownsAfterItAreEmpty)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on second dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on second dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting A (0x41) on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting A (0x41) on second dropdown of first column of empty 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting A (0x41) on second dropdown of second column of empty 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 6: Validate the element when selecting A (0x41) on second dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and ClearUnusedDropDowns action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting first drop down to an action key on a hybrid column if all the drop downs after it are empty
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingFirstDropDownToActionKeyOnAHybridColumnIfAllTheDropDownsAfterItAreEmpty)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on first dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList), -1, -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and ClearUnusedDropDowns action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and AddDropDown action is required on setting last drop down to a non-repeated modifier key on a column there are less than 3 drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndAddDropDownAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column of 2 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 2 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 1 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 6: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 7: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown hybrid shortcut to key row with an action key selected
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), 0x44 }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and AddDropDown action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::AddDropDown);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting last drop down to a repeated modifier key on a column there are less than 3 drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column of 2 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a non repeated modifier key on a column there 3 or more drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 3 dropdown shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 3 dropdown shortcut to shortcut row with first two empty
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ -1, -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown shortcut to shortcut row with first two empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ -1, -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 6: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first two empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ -1, -1, GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 7: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 8: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 9: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a repeated modifier key on a column there 3 or more drop downs
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of first column of 3 dropdown shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 3 dropdown shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with middle empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 5: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 6: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutMaxShortcutSizeOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and no action is required on setting non-last drop down to a non repeated modifier key on a column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingNonLastDropDownToNonRepeatedModifierKeyOnAColumn)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 2 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 2 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 5: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 6: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 7: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 8: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 9: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting non-last drop down to a repeated modifier key on a column
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingNonLastDropDownToRepeatedModifierKeyOnAColumn)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column of 3 dropdown shortcut to shortcut row with first empty
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 2: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column of 3 dropdown shortcut to shortcut row with first empty
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 3: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first empty
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting Alt (VK_MENU) on first dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 5: Validate the element when selecting Alt (VK_MENU) on first dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 6: Validate the element when selecting Alt (VK_MENU) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 7: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 8: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column of 3 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 9: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutCannotHaveRepeatedModifier);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no action is required on setting first drop down to None on a non-hybrid column with one drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithOneDropDown)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of first column of 1 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on first dropdown of second column of 1 dropdown shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutStartWithModifier);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting first drop down to None on a hybrid column with one drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyErrorAndNoAction_OnSettingFirstDropDownToNoneOnHybridColumnWithOneDropDown)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of first column of 1 dropdown hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutAtleast2Keys error and no action is required on setting first drop down to None on a non-hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutAtleast2KeysAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of first column of 2 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on first dropdown of second column of 2 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting None (0) on first dropdown of second column of 2 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on first dropdown of second column of 2 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutAtleast2Keys);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting second drop down to None on a non-hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyAndNoAction_OnSettingSecondDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on second dropdown of first column of 2 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ -1, 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on second dropdown of second column of 2 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting None (0) on second dropdown of second column of 2 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on second dropdown of second column of 2 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting drop down to None on a hybrid column with two drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndDeleteDropDownAction_OnSettingDropDownToNoneOnHybridColumnWithTwoDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on second dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, 0 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting None (0) on first dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on second dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0 }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and DeleteDropDown action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting non last drop down to None on a column with three drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndDeleteDropDownAction_OnSettingNonLastDropDownToNoneOnColumnWithThreeDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of first column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0, -1, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, -1, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, -1, -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on second dropdown of first column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ -1, 0, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting None (0) on second dropdown of second column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, 0, -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 6: Validate the element when selecting None (0) on second dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, 0, -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 7: Validate the element when selecting None (0) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 8: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 9: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 10: Validate the element when selecting None (0) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 11: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 12: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x42, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid and DeleteDropDown action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::DeleteDropDown);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting last drop down to None on a column with three drop down
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNoneOnColumnWithThreeDropDowns)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting None (0) on first dropdown of first column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ -1, -1, 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown empty shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ -1, -1, 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 3: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ -1, -1, 0 }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), Shortcut() }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 5: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), 0 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });
            // Case 6: Validate the element when selecting None (0) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), 0 }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_MENU, 0x42 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid and no action is required
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutOneActionKey);
                Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns WinL error on setting a drop down to Win or L on a column resulting in Win+L
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnWinLError_OnSettingDropDownToWinOrLOnColumnResultingInWinL)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting L (0x4C) on second dropdown of first column of LWin+Empty shortcut
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LWIN }, Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting L (0x4C) on second dropdown of second column of LWin+Empty shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN } }, std::wstring()) });
            // Case 3: Validate the element when selecting L (0x4C) on second dropdown of second column of hybrid LWin+Empty shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN } }, std::wstring()) });
            // Case 4: Validate the element when selecting L (0x4C) on second dropdown of first column of Win+Empty shortcut
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(CommonSharedConstants::VK_WIN_BOTH, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ CommonSharedConstants::VK_WIN_BOTH }, Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting L (0x4C) on second dropdown of second column of Win+Empty shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(CommonSharedConstants::VK_WIN_BOTH, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ CommonSharedConstants::VK_WIN_BOTH } }, std::wstring()) });
            // Case 6: Validate the element when selecting L (0x4C) on second dropdown of second column of hybrid Win+Empty shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(CommonSharedConstants::VK_WIN_BOTH, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ CommonSharedConstants::VK_WIN_BOTH } }, std::wstring()) });
            // Case 7: Validate the element when selecting LWin (VK_LWIN) on first dropdown of first column of Empty+L shortcut
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x4C }, Shortcut() }, std::wstring()) });
            // Case 8: Validate the element when selecting LWin (VK_LWIN) on first dropdown of second column of Empty+L shortcut
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ 0x4C } }, std::wstring()) });
            // Case 9: Validate the element when selecting LWin (VK_LWIN) on first dropdown of second column of hybrid Empty+L shortcut
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ 0x4C } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::WinL);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns WinL error on setting a drop down to null or none on a column resulting in Win+L
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnWinLError_OnSettingDropDownToNullOrNoneOnColumnResultingInWinL)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Null (-1) on second dropdown of first column of LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), -1, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C }, Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting Null (-1) on second dropdown of second column of LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), -1, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C } }, std::wstring()) });
            // Case 3: Validate the element when selecting Null (-1) on second dropdown of second column of hybrid LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), -1, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C } }, std::wstring()) });
            // Case 4: Validate the element when selecting None (0) on second dropdown of first column of LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), 0, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C }, Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting None (0) on second dropdown of second column of LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), 0, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C } }, std::wstring()) });
            // Case 6: Validate the element when selecting None (0) on second dropdown of second column of hybrid LWin + Ctrl + L shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LWIN, keyList), 0, GetDropDownIndexFromDropDownList(0x4C, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_LWIN, VK_CONTROL, 0x4C } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::WinL);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns CtrlAltDel error on setting a drop down to Ctrl, Alt or Del on a column resulting in Ctrl+Alt+Del
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnCtrlAltDelError_OnSettingDropDownToCtrlAltOrDelOnColumnResultingInCtrlAltDel)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting Del (VK_DELETE) on third dropdown of first column of Ctrl+Alt+Empty shortcut
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_MENU }, Shortcut() }, std::wstring()) });
            // Case 2: Validate the element when selecting Del (VK_DELETE) on third dropdown of second column of Ctrl+Alt+Empty shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, VK_MENU } }, std::wstring()) });
            // Case 3: Validate the element when selecting Del (VK_DELETE) on third dropdown of second column of hybrid Ctrl+Alt+Empty shortcut
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, VK_MENU } }, std::wstring()) });
            // Case 4: Validate the element when selecting Alt (VK_MENU) on second dropdown of first column of Ctrl+Empty+Del shortcut
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_DELETE }, Shortcut() }, std::wstring()) });
            // Case 5: Validate the element when selecting Alt (VK_MENU) on second dropdown of second column of Ctrl+Empty+Del shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, VK_DELETE } }, std::wstring()) });
            // Case 6: Validate the element when selecting Alt (VK_MENU) on second dropdown of second column of hybrid Ctrl+Empty+Del shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_MENU, keyList), GetDropDownIndexFromDropDownList(VK_DELETE, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ Shortcut(), std::vector<DWORD>{ VK_CONTROL, VK_DELETE } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::CtrlAltDel);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns MapToSameKey error on setting hybrid second column to match first column in a remap keys table
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameKeyError_OnSettingHybridSecondColumnToFirstColumnInKeyTable)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1: Validate the element when selecting A (0x41) on first dropdown of empty hybrid second column
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList), -1, -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, NULL }, std::wstring()) });
            // Case 2: Validate the element when selecting A (0x41) on second dropdown of empty hybrid second column
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(0x41, keyList), -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, NULL }, std::wstring()) });
            // Case 3: Validate the element when selecting A (0x41) on third dropdown of empty hybrid second column
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ -1, -1, GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, NULL }, std::wstring()) });
            // Case 4: Validate the element when selecting A (0x41) on first dropdown of hybrid second column with key
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, 0x43 }, std::wstring()) });
            // Case 5: Validate the element when selecting Null (-1) on first dropdown of hybrid second column with shortcut
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 6: Validate the element when selecting None (0) on first dropdown of hybrid second column with shortcut
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(0x41, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ 0x41, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 7: Validate the element when selecting Null (-1) on second dropdown of hybrid second column with shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ VK_CONTROL, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 8: Validate the element when selecting None (0) on second dropdown of hybrid second column with shortcut
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ VK_CONTROL, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::MapToSameKey);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns MapToSameShortcut error on setting one column to match the other and both are valid 3 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameShortcutError_OnSettingOneColumnToTheOtherAndBothAreValid3KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Shift+Empty
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of second column with Ctrl+Shift+Empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT } }, std::wstring()) });
            // Case 3 : Validate the element when selecting C (0x43) on third dropdown of second column with hybrid Ctrl+Shift+Empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT } }, std::wstring()) });
            // Case 4 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 5 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column with Ctrl+Empty+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 6 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column with hybrid Ctrl+Empty+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 7 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 8 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column with Empty+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 9 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column with hybrid Empty+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::MapToSameShortcut);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns MapToSameShortcut error on setting one column to match the other and both are valid 2 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameShortcutError_OnSettingOneColumnToTheOtherAndBothAreValid2KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Empty+Empty
            testCases.push_back({ 0, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of second column with Ctrl+Empty+Empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL } }, std::wstring()) });
            // Case 3 : Validate the element when selecting C (0x43) on third dropdown of second column with hybrid Ctrl+Empty+Empty
            testCases.push_back({ 0, 1, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL } }, std::wstring()) });
            // Case 4 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+Empty+Empty
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 5 : Validate the element when selecting C (0x43) on second dropdown of second column with Ctrl+Empty+Empty
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL } }, std::wstring()) });
            // Case 6 : Validate the element when selecting C (0x43) on second dropdown of second column with hybrid Ctrl+Empty+Empty
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL } }, std::wstring()) });
            // Case 7 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 8 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with Empty+Empty+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ 0x43 } }, std::wstring()) });
            // Case 9 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with hybrid Empty+Empty+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ 0x43 } }, std::wstring()) });
            // Case 10 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 11 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column with Empty+Empty+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ 0x43 } }, std::wstring()) });
            // Case 12 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column with hybrid Empty+Empty+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ 0x43 } }, std::wstring()) });
            // Case 13 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+A
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x41 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 14 : Validate the element when selecting C (0x43) on second dropdown of second column with Ctrl+A
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 15 : Validate the element when selecting C (0x43) on second dropdown of second column with hybrid Ctrl+A
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x41 } }, std::wstring()) });
            // Case 16 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Alt+C
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_MENU, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 17 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with Alt+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_MENU, 0x43 } }, std::wstring()) });
            // Case 18 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with hybrid Alt+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_MENU, 0x43 } }, std::wstring()) });
            // Case 19 : Validate the element when selecting Null (-1)  on second dropdown of first column with Ctrl+Shift+C
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 20 : Validate the element when selecting Null (-1)  on second dropdown of second column with Ctrl+Shift+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 21 : Validate the element when selecting Null (-1)  on second dropdown of second column with hybrid Ctrl+Shift+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 22 : Validate the element when selecting None (0)  on second dropdown of first column with Ctrl+Shift+C
            testCases.push_back({ 0, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 23 : Validate the element when selecting None (0)  on second dropdown of second column with Ctrl+Shift+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 24 : Validate the element when selecting None (0)  on second dropdown of second column with hybrid Ctrl+Shift+C
            testCases.push_back({ 0, 1, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 } }, std::wstring()) });
            // Case 25 : Validate the element when selecting Null (-1)  on first dropdown of first column with Shift+Ctrl+C
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 26 : Validate the element when selecting Null (-1)  on first dropdown of second column with Shift+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 27 : Validate the element when selecting Null (-1)  on first dropdown of second column with hybrid Shift+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 28 : Validate the element when selecting None (0)  on first dropdown of first column with Shift+Ctrl+C
            testCases.push_back({ 0, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 29 : Validate the element when selecting None (0)  on first dropdown of second column with Shift+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 } }, std::wstring()) });
            // Case 30 : Validate the element when selecting None (0)  on first dropdown of second column with hybrid Shift+Ctrl+C
            testCases.push_back({ 0, 1, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), true, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 } }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::MapToSameShortcut);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns SameShortcutPreviouslyMapped error on setting first column to match first column in another row with same target app and both are valid 3 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnSameShortcutPreviouslyMappedError_OnSettingFirstColumnToFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid3KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Shift+Empty
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT }, Shortcut() }, std::wstring()) });
            // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 3 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+Shift+C remapped
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::SameShortcutPreviouslyMapped);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error on setting first column to match first column in another row with different target app and both are valid 3 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid3KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Shift+Empty for testApp2
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT }, Shortcut() }, testApp2) });
            // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, testApp2) });
            // Case 3 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, testApp2) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+Shift+C remapped for testApp1
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp1));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ConflictingModifierShortcut error on setting first column to conflict with first column in another row with same target app and both are valid 3 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnConflictingModifierShortcutError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid3KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with LCtrl+Shift+Empty
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT }, Shortcut() }, std::wstring()) });
            // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with LCtrl+Empty+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 3 : Validate the element when selecting LShift (VK_LSHIFT) on first dropdown of first column with Empty+Ctrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LSHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+Shift+C remapped
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ConflictingModifierShortcut);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error on setting first column to conflict with first column in another row with different target app and both are valid 3 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid3KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on third dropdown of first column with LCtrl+Shift+Empty for testApp2
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT }, Shortcut() }, testApp2) });
            // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with LCtrl+Empty+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(VK_SHIFT, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, 0x43 }, Shortcut() }, testApp2) });
            // Case 3 : Validate the element when selecting LShift (VK_LSHIFT) on first dropdown of first column with Empty+Ctrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LSHIFT, keyList), GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, testApp2) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+Shift+C remapped for testApp1
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp1));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns SameShortcutPreviouslyMapped error on setting first column to match first column in another row with same target app and both are valid 2 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnSameShortcutPreviouslyMappedError_OnSettingFirstColumnToFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid2KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+Empty
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, std::wstring()) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Empty+Empty
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, std::wstring()) });
            // Case 3 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+Empty+Empty
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, std::wstring()) });
            // Case 4 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 5 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 6 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 7 : Validate the element when selecting Null (-1) on second dropdown of first column with Ctrl+Shift+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 8 : Validate the element when selecting Null (-1) on first dropdown of first column with Shift+Ctrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 9 : Validate the element when selecting None (0) on second dropdown of first column with Ctrl+Shift+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 10 : Validate the element when selecting None (0) on first dropdown of first column with Shift+Ctrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+C remapped
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::SameShortcutPreviouslyMapped);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error on setting first column to match first column in another row with different target app and both are valid 2 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid2KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+Empty for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, testApp2) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of first column with Ctrl+Empty+Empty for testApp2
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, testApp2) });
            // Case 3 : Validate the element when selecting C (0x43) on second dropdown of first column with Ctrl+Empty+Empty for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL }, Shortcut() }, testApp2) });
            // Case 4 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 5 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 6 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 7 : Validate the element when selecting Null (-1) on second dropdown of first column with Ctrl+Shift+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp2) });
            // Case 8 : Validate the element when selecting Null (-1) on first dropdown of first column with Shift+Ctrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, Shortcut() }, testApp2) });
            // Case 9 : Validate the element when selecting None (0) on second dropdown of first column with Ctrl+Shift+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp2) });
            // Case 10 : Validate the element when selecting None (0) on first dropdown of first column with Shift+Ctrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_CONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_CONTROL, 0x43 }, Shortcut() }, testApp2) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+C remapped for testApp1
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, testApp1));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns ConflictingModifierShortcut error on setting first column to conflict with first column in another row with same target app and both are valid 2 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnConflictingModifierShortcutError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid2KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on second dropdown of first column with LCtrl+Empty
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, std::wstring()) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of first column with LCtrl+Empty+Empty
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, std::wstring()) });
            // Case 3 : Validate the element when selecting C (0x43) on second dropdown of first column with LCtrl+Empty+Empty
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, std::wstring()) });
            // Case 4 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 5 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+Empty+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 6 : Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column with Empty+Empty+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, std::wstring()) });
            // Case 7 : Validate the element when selecting Null (-1) on second dropdown of first column with LCtrl+Shift+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 8 : Validate the element when selecting Null (-1) on first dropdown of first column with Shift+LCtrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_LCONTROL, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 9 : Validate the element when selecting None (0) on second dropdown of first column with LCtrl+Shift+C
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT, 0x43 }, Shortcut() }, std::wstring()) });
            // Case 10 : Validate the element when selecting None (0) on first dropdown of first column with Shift+LCtrl+C
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, std::wstring(), false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_LCONTROL, 0x43 }, Shortcut() }, std::wstring()) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+C remapped
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, std::wstring()));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is invalid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ConflictingModifierShortcut);
            });
        }

        // Test if the ValidateShortcutBufferElement method returns no error on setting first column to conflict with first column in another row with different target app and both are valid 2 key shortcuts
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid2KeyShortcuts)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);

            std::vector<ValidateShortcutBufferElementArgs> testCases;
            // Case 1 : Validate the element when selecting C (0x43) on second dropdown of first column with LCtrl+Empty for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, testApp2) });
            // Case 2 : Validate the element when selecting C (0x43) on third dropdown of first column with LCtrl+Empty+Empty for testApp2
            testCases.push_back({ 1, 0, 2, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, testApp2) });
            // Case 3 : Validate the element when selecting C (0x43) on second dropdown of first column with LCtrl+Empty+Empty for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList), -1 }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL }, Shortcut() }, testApp2) });
            // Case 4 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 5 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+Empty+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 6 : Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column with Empty+Empty+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ 0x43 }, Shortcut() }, testApp2) });
            // Case 7 : Validate the element when selecting Null (-1) on second dropdown of first column with LCtrl+Shift+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), -1, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp2) });
            // Case 8 : Validate the element when selecting Null (-1) on first dropdown of first column with Shift+LCtrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ -1, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_LCONTROL, 0x43 }, Shortcut() }, testApp2) });
            // Case 9 : Validate the element when selecting None (0) on second dropdown of first column with LCtrl+Shift+C for testApp2
            testCases.push_back({ 1, 0, 1, std::vector<int32_t>{ GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), 0, GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_LCONTROL, VK_SHIFT, 0x43 }, Shortcut() }, testApp2) });
            // Case 10 : Validate the element when selecting None (0) on first dropdown of first column with Shift+LCtrl+C for testApp2
            testCases.push_back({ 1, 0, 0, std::vector<int32_t>{ 0, GetDropDownIndexFromDropDownList(VK_LCONTROL, keyList), GetDropDownIndexFromDropDownList(0x43, keyList) }, testApp2, false, std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, VK_LCONTROL, 0x43 }, Shortcut() }, testApp2) });

            RunTestCases(testCases, [this, &keyList](const ValidateShortcutBufferElementArgs& testCase) {
                // Arrange
                RemapBuffer remapBuffer;
                // Ctrl+C remapped for testApp1
                remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_CONTROL, 0x43 }, Shortcut() }, testApp1));
                remapBuffer.push_back(testCase.bufferRow);

                // Act
                std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedIndicesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, keyList, remapBuffer, true);

                // Assert that the element is valid
                Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::NoError);
            });
        }

        // Return error on Disable as second modifier key or action key
        TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnDisableAsActionKeyError_OnSettingSecondDropdownAsDisable)
        {
            std::vector<DWORD> keyList = keyboardLayout.GetKeyCodeList(true);
            keyList.insert(keyList.begin(), CommonSharedConstants::VK_DISABLED);
            // Arrange
            RemapBuffer remapBuffer;
            remapBuffer.push_back(std::make_pair(RemapBufferItem{ std::vector<DWORD>{ VK_SHIFT, CommonSharedConstants::VK_DISABLED }, Shortcut() }, testApp1));
            std::vector<int32_t> selectedIndices = { 
                GetDropDownIndexFromDropDownList(VK_SHIFT, keyList),
                GetDropDownIndexFromDropDownList(CommonSharedConstants::VK_DISABLED, keyList)
            };

            // Act
            std::pair<KeyboardManagerHelper::ErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, selectedIndices, testApp1, true, keyList, remapBuffer, true);

            // Assert
            Assert::AreEqual(true, result.first == KeyboardManagerHelper::ErrorType::ShortcutDisableAsActionKey);
            Assert::AreEqual(true, result.second == BufferValidationHelpers::DropDownAction::NoAction);
        }
    };
}