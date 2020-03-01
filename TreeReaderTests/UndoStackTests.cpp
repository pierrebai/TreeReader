#include "UndoStack.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TreeReader;

namespace TreeReaderTests
{		
	TEST_CLASS(UndoStackTests)
	{
	public:
		
		TEST_METHOD(UndoStackUndoRedo)
		{
         UndoStack undo;

         Assert::IsFalse(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         Assert::AreEqual<size_t>(0, undo.Contents().size());

         struct Data
         {
            Data(double a) : a(a), a_squared(a*a) { }

            double a = 0.;
            double a_squared = 0.;
         };

         Data my_data(7.);

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         undo.Commit(
         {
            my_data,
            [&](std::any& d)
            {
               std::any_cast<Data&>(d).a_squared = 0.;
            },
            [&my_data=my_data](const std::any& d)
            {
               my_data = std::any_cast<const Data&>(d);
               my_data.a_squared = my_data.a * my_data.a;
            }
         });

         Assert::IsFalse(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         Assert::AreEqual<size_t>(1, undo.Contents().size());
         Assert::AreEqual(7., std::any_cast<const Data&>(undo.Contents().back().Data).a);
         Assert::AreEqual(0., std::any_cast<const Data&>(undo.Contents().back().Data).a_squared);

         my_data = Data(9.);

         undo.Commit(
         {
            my_data,
            [&](std::any& d)
            {
               std::any_cast<Data&>(d).a_squared = 0.;
            },
            [&my_data=my_data](const std::any& d)
            {
               my_data = std::any_cast<const Data&>(d);
               my_data.a_squared = my_data.a * my_data.a;
            }
         });

         Assert::IsTrue(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         Assert::AreEqual(9., my_data.a);
         Assert::AreEqual(81., my_data.a_squared);

         Assert::AreEqual<size_t>(2, undo.Contents().size());
         Assert::AreEqual(9., std::any_cast<const Data&>(undo.Contents().back().Data).a);
         Assert::AreEqual(0., std::any_cast<const Data&>(undo.Contents().back().Data).a_squared);

         undo.Undo();

         Assert::IsFalse(undo.HasUndo());
         Assert::IsTrue(undo.HasRedo());

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         undo.Redo();

         Assert::IsTrue(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         Assert::AreEqual(9., my_data.a);
         Assert::AreEqual(81., my_data.a_squared);
      }

		TEST_METHOD(UndoStack_without_deaded_awaken)
		{
         UndoStack undo;

         undo.Commit({ 1.5 });
         undo.Commit({ 3.5 });

         Assert::IsTrue(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         undo.Undo();

         Assert::IsFalse(undo.HasUndo());
         Assert::IsTrue(undo.HasRedo());

         undo.Undo();

         Assert::IsFalse(undo.HasUndo());
         Assert::IsTrue(undo.HasRedo());

         undo.Redo();

         Assert::IsTrue(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());

         undo.Redo();

         Assert::IsTrue(undo.HasUndo());
         Assert::IsFalse(undo.HasRedo());
      }

		TEST_METHOD(UndoStack_empty_noop)
		{
         UndoStack undo;

         undo.Undo();
         undo.Undo();
         undo.Undo();

         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();

         undo.Undo();
         undo.Undo();

         undo.Redo();
         undo.Redo();

         undo.Commit({ 1.5 });
         undo.Commit({ 3.5 });

         undo.Redo();
         undo.Redo();

         undo.Undo();
         undo.Undo();
         undo.Undo();

         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();
         undo.Redo();

         undo.Undo();
         undo.Undo();

         undo.Redo();
         undo.Redo();
      }
	};
}
