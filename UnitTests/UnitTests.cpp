#include "pch.h"
#include "CppUnitTest.h"
#include <cmath>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../VectorTransforms/Functions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VectorTransformsTests {

    constexpr double EPS = 1e-12;

    template <typename Ex, typename Fn>
    std::string messageOf(Fn fn) {
        try {
            fn();
        } catch (const Ex& e) {
            return e.what();
        }
        Assert::Fail(L"Expected exception was not thrown");
        return "";
    }

    template <typename T>
    Vector<T> makeVector(std::initializer_list<T> values) {
        Vector<T> v(static_cast<int>(values.size()));
        int idx = 0;
        for (const T& val : values) {
            v[idx++] = val;
        }
        return v;
    }

    template <typename T>
    void assertVectorEqual(const Vector<T>& expected, const Vector<T>& actual) {
        Assert::AreEqual(expected.size(), actual.size());
        for (int i = 0; i < expected.size(); ++i) {
            Assert::AreEqual(expected[i], actual[i]);
        }
    }

    template <>
    void assertVectorEqual<double>(const Vector<double>& expected, const Vector<double>& actual) {
        Assert::AreEqual(expected.size(), actual.size());
        for (int i = 0; i < expected.size(); ++i) {
            Assert::IsTrue(std::fabs(expected[i] - actual[i]) <= EPS);
        }
    }

    // 1. VectorTests (7 тестів)
    TEST_CLASS(VectorTests) {
    public:
        TEST_METHOD(DefaultAndSizedZeroInit) {
            Vector<double> empty;
            Assert::AreEqual(0, empty.size());
            Vector<double> v(4);
            Assert::AreEqual(4, v.size());
            for (int i = 0; i < 4; ++i) {
                Assert::AreEqual(0.0, v[i], EPS);
            }
        }

        TEST_METHOD(NegativeSizeThrows) {
            std::string msg = messageOf<std::invalid_argument>([]() { Vector<double> v(-1); });
            Assert::AreEqual(std::string("invalid size"), msg);
        }

        TEST_METHOD(IndexOutOfRangeThrows) {
            Vector<double> v(3);
            messageOf<std::out_of_range>([&]() { (void)v[-1]; });
            messageOf<std::out_of_range>([&]() { (void)v[3]; });
        }

        TEST_METHOD(CopyConstructorDeepCopy) {
            Vector<double> a = makeVector<double>({ 1.0, 2.0, 3.0 });
            Vector<double> b(a);
            b[0] = 99.0;
            Assert::AreEqual(1.0, a[0], EPS);
            Assert::AreEqual(99.0, b[0], EPS);
        }

        TEST_METHOD(AssignmentDeepCopy) {
            Vector<double> a = makeVector<double>({ 4.0, 5.0 });
            Vector<double> b(1);
            b = a;
            b[1] = -7.0;
            Assert::AreEqual(5.0, a[1], EPS);
            Assert::AreEqual(-7.0, b[1], EPS);
        }

        TEST_METHOD(SelfAssignmentSafe) {
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0 });
            Vector<double>& ref = a;
            a = ref;
            assertVectorEqual(makeVector<double>({ 3.0, -1.0, 4.0 }), a);
        }

        TEST_METHOD(TemplateWorksWithInt) {
            Vector<int> v = makeVector<int>({ 10, 20, 30 });
            Assert::AreEqual(3, v.size());
            Assert::AreEqual(20, v[1]);
        }
    };

    // 2. ReflectTests (5 тестів)
    TEST_CLASS(ReflectTests) {
    public:
        TEST_METHOD(Chapter1Example) {
            VectorReflect<double> op;
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            assertVectorEqual(makeVector<double>({ 2.0, 4.0, -1.0, 3.0 }), op.execute(a));
        }

        TEST_METHOD(OddLengthVector) {
            VectorReflect<double> op;
            Vector<double> a = makeVector<double>({ 1.0, 2.0, 3.0, 4.0, 5.0 });
            assertVectorEqual(makeVector<double>({ 5.0, 4.0, 3.0, 2.0, 1.0 }), op.execute(a));
        }

        TEST_METHOD(SingleElement) {
            VectorReflect<double> op;
            Vector<double> a = makeVector<double>({ 7.0 });
            assertVectorEqual(makeVector<double>({ 7.0 }), op.execute(a));
        }

        TEST_METHOD(DoubleReflectReturnsOriginal) {
            VectorReflect<double> op;
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            assertVectorEqual(a, op.execute(op.execute(a)));
        }

        TEST_METHOD(DoesNotModifyInput) {
            VectorReflect<double> op;
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            (void)op.execute(a);
            assertVectorEqual(makeVector<double>({ 3.0, -1.0, 4.0, 2.0 }), a);
        }
    };

    // 3. ScaleTests (5 тестів)
    TEST_CLASS(ScaleTests) {
    public:
        TEST_METHOD(ScaleByTwoPointFive) {
            VectorScale<double> op(2.5);
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            assertVectorEqual(makeVector<double>({ 7.5, -2.5, 10.0, 5.0 }), op.execute(a));
        }

        TEST_METHOD(ScaleByZero) {
            VectorScale<double> op(0.0);
            Vector<double> a = makeVector<double>({ 5.0, -5.0, 0.5 });
            assertVectorEqual(makeVector<double>({ 0.0, 0.0, 0.0 }), op.execute(a));
        }

        TEST_METHOD(ScaleByOne) {
            VectorScale<double> op(1.0);
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            assertVectorEqual(a, op.execute(a));
        }

        TEST_METHOD(ScaleByMinusOne) {
            VectorScale<double> op(-1.0);
            Vector<double> a = makeVector<double>({ 0.0, 2.0, -2.0 });
            assertVectorEqual(makeVector<double>({ 0.0, -2.0, 2.0 }), op.execute(a));
        }

        TEST_METHOD(DistributivityOverScalars) {
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            Vector<double> left = VectorAdd<double>(VectorScale<double>(3.0).execute(a))
                                      .execute(VectorScale<double>(2.0).execute(a));
            Vector<double> right = VectorScale<double>(5.0).execute(a);
            assertVectorEqual(right, left);
        }
    };

    // 4. AddTests (5 тестів)
    TEST_CLASS(AddTests) {
    public:
        TEST_METHOD(Chapter1Example) {
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            Vector<double> b = makeVector<double>({ 1.0, 5.0, -2.0, 0.5 });
            VectorAdd<double> op(b);
            assertVectorEqual(makeVector<double>({ 4.0, 4.0, 2.0, 2.5 }), op.execute(a));
        }

        TEST_METHOD(Commutativity) {
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            Vector<double> b = makeVector<double>({ 1.0, 5.0, -2.0, 0.5 });
            assertVectorEqual(VectorAdd<double>(b).execute(a), VectorAdd<double>(a).execute(b));
        }

        TEST_METHOD(AdditiveInverseGivesZero) {
            Vector<double> a = makeVector<double>({ 3.0, -1.0, 4.0, 2.0 });
            Vector<double> neg = VectorScale<double>(-1.0).execute(a);
            assertVectorEqual(makeVector<double>({ 0.0, 0.0, 0.0, 0.0 }),
                              VectorAdd<double>(neg).execute(a));
        }

        TEST_METHOD(DifferentSizesThrow) {
            Vector<double> a = makeVector<double>({ 1.0, 2.0, 3.0 });
            Vector<double> b = makeVector<double>({ 4.0, 5.0 });
            VectorAdd<double> op(b);
            std::string msg = messageOf<std::invalid_argument>([&]() { (void)op.execute(a); });
            Assert::AreEqual(std::string("vector sizes differ: 3 and 2"), msg);
        }

        TEST_METHOD(SecondVectorCopyIndependent) {
            Vector<double> a = makeVector<double>({ 1.0, 2.0 });
            Vector<double> b = makeVector<double>({ 3.0, 4.0 });
            VectorAdd<double> op(b);
            b[0] = 100.0;
            assertVectorEqual(makeVector<double>({ 4.0, 6.0 }), op.execute(a));
        }
    };

    // 5. TransformTests (5 тестів)
    TEST_CLASS(TransformTests) {
    public:
        TEST_METHOD(EmptyVectorThrowsInExecute) {
            VectorReflect<double> op;
            Vector<double> empty;
            std::string msg = messageOf<std::invalid_argument>([&]() { (void)op.execute(empty); });
            Assert::AreEqual(std::string("invalid size"), msg);
        }

        TEST_METHOD(PolymorphicCallThroughBasePointer) {
            std::unique_ptr<Transform<double>> t = std::make_unique<VectorScale<double>>(2.0);
            Vector<double> a = makeVector<double>({ 1.5, -2.0 });
            assertVectorEqual(makeVector<double>({ 3.0, -4.0 }), t->execute(a));
        }

        TEST_METHOD(CreateTransformForValidOps) {
            Task<double> t1; t1.op = "REFLECT";
            Task<double> t2; t2.op = "SCALE"; t2.k = 3.0;
            Task<double> t3; t3.op = "ADD"; t3.b = makeVector<double>({ 1.0, 1.0 });
            Vector<double> a = makeVector<double>({ 2.0, 5.0 });
            assertVectorEqual(makeVector<double>({ 5.0, 2.0 }), createTransform(t1)->execute(a));
            assertVectorEqual(makeVector<double>({ 6.0, 15.0 }), createTransform(t2)->execute(a));
            assertVectorEqual(makeVector<double>({ 3.0, 6.0 }), createTransform(t3)->execute(a));
        }

        TEST_METHOD(CreateTransformUnknownOpThrows) {
            Task<double> t;
            t.op = "MUL";
            std::string msg = messageOf<std::invalid_argument>([&]() { (void)createTransform(t); });
            Assert::AreEqual(std::string("unknown operation"), msg);
        }

        TEST_METHOD(TemplatesWithIntType) {
            Task<int> t;
            t.op = "SCALE";
            t.k = 4;
            Vector<int> a = makeVector<int>({ 2, -3, 5 });
            assertVectorEqual(makeVector<int>({ 8, -12, 20 }), createTransform(t)->execute(a));
        }
    };

    // 6. ParserTests (5 тестів)
    TEST_CLASS(ParserTests) {
    public:
        TEST_METHOD(ParseNumberValid) {
            double v = 0.0;
            Assert::IsTrue(parseNumber<double>("2.5", v));
            Assert::AreEqual(2.5, v, EPS);
            Assert::IsTrue(parseNumber<double>("-1e3", v));
            Assert::AreEqual(-1000.0, v, EPS);
        }

        TEST_METHOD(ParseNumberRejectsInvalid) {
            double v = 42.0;
            Assert::IsFalse(parseNumber<double>("2,5", v));
            Assert::IsFalse(parseNumber<double>("1.2.3", v));
            Assert::IsFalse(parseNumber<double>("5x", v));
            Assert::IsFalse(parseNumber<double>("1e999", v));
            Assert::AreEqual(42.0, v, EPS);
        }

        TEST_METHOD(ParseSizeValidation) {
            int n = 0;
            Assert::IsTrue(parseSize("4", n));
            Assert::AreEqual(4, n);
            Assert::IsFalse(parseSize("0", n));
            Assert::IsFalse(parseSize("-3", n));
            Assert::IsFalse(parseSize("2.5", n));
        }

        TEST_METHOD(ParseVectorLineValid) {
            Vector<double> v;
            std::string err;
            Assert::IsTrue(parseVectorLine<double>("3 1.5 -2 4", v, err));
            assertVectorEqual(makeVector<double>({ 1.5, -2.0, 4.0 }), v);
        }

        TEST_METHOD(ParseVectorLineErrors) {
            Vector<double> v;
            std::string err;
            Assert::IsFalse(parseVectorLine<double>("0", v, err));
            Assert::AreEqual(std::string("invalid size"), err);
            Assert::IsFalse(parseVectorLine<double>("3 1 2", v, err));
            Assert::AreEqual(std::string("wrong number of elements"), err);
            Assert::IsFalse(parseVectorLine<double>("2 1 x", v, err));
            Assert::AreEqual(std::string("invalid number"), err);
        }
    };

    // 7. ReadBlockTests (5 тестів)
    TEST_CLASS(ReadBlockTests) {
    public:
        TEST_METHOD(ReadsAllThreeOperations) {
            std::istringstream in("REFLECT\n2 1 2\n\nSCALE 2.5\n2 3 4\n\nADD\n2 1 2\n2 3 4\n");
            Task<double> task;
            std::string err;
            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string("REFLECT"), task.op);
            Assert::AreEqual(std::string(""), err);

            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string("SCALE"), task.op);
            Assert::AreEqual(2.5, task.k, EPS);
            Assert::AreEqual(std::string(""), err);

            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string("ADD"), task.op);
            Assert::AreEqual(std::string(""), err);
        }

        TEST_METHOD(DetectsAllFormatErrors) {
            Task<double> task;
            std::string err;
            {
                std::istringstream in("MUL\n2 1 2\n");
                Assert::IsTrue(readBlock(in, task, err));
                Assert::AreEqual(std::string("unknown operation"), err);
            }
            {
                std::istringstream in("SCALE 2,5\n2 1 2\n");
                Assert::IsTrue(readBlock(in, task, err));
                Assert::AreEqual(std::string("invalid constant"), err);
            }
            {
                std::istringstream in("REFLECT extra\n2 1 2\n");
                Assert::IsTrue(readBlock(in, task, err));
                Assert::AreEqual(std::string("unexpected parameter"), err);
            }
            {
                std::istringstream in("ADD\n2 1 2\n");
                Assert::IsTrue(readBlock(in, task, err));
                Assert::AreEqual(std::string("missing second vector"), err);
            }
        }

        TEST_METHOD(HandlesBlankLinesAndCRLF) {
            std::istringstream in("\r\n\r\nREFLECT\r\n2 3 4\r\n\r\n");
            Task<double> task;
            std::string err;
            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string(""), err);
            assertVectorEqual(makeVector<double>({ 3.0, 4.0 }), task.a);
        }

        TEST_METHOD(StopsOnEndAndEof) {
            std::istringstream in("END\nREFLECT\n1 5\n");
            Task<double> task;
            std::string err;
            Assert::IsFalse(readBlock(in, task, err));
        }

        TEST_METHOD(ContinuesAfterErrorBlock) {
            std::istringstream in("SCALE bad\n2 1 2\n\nREFLECT\n2 7 8\n");
            Task<double> task;
            std::string err;
            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string("invalid constant"), err);
            Assert::IsTrue(readBlock(in, task, err));
            Assert::AreEqual(std::string(""), err);
            Assert::AreEqual(std::string("REFLECT"), task.op);
        }
    };

    // 8. FormatTests (5 тестів)
    TEST_CLASS(FormatTests) {
    public:
        TEST_METHOD(FourDecimalPlaces) {
            Assert::AreEqual(std::string("2.5000"), formatValue(2.5));
            Assert::AreEqual(std::string("-1.0000"), formatValue(-1.0));
        }

        TEST_METHOD(HidesNegativeZero) {
            Assert::AreEqual(std::string("0.0000"), formatValue(-0.0));
            Assert::AreEqual(std::string("0.0000"), formatValue(-1e-9));
        }

        TEST_METHOD(FormatsConstant) {
            Assert::AreEqual(std::string("2.5"), formatConstant(2.5));
            Assert::AreEqual(std::string("0"), formatConstant(-0.0));
        }

        TEST_METHOD(WriteResultFormat) {
            Task<double> task;
            task.op = "SCALE";
            task.k = 2.5;
            Vector<double> c = makeVector<double>({ 7.5, -2.5 });
            std::ostringstream out;
            writeResult(out, 2, task, c);
            Assert::AreEqual(std::string("2 SCALE 2.5 = 7.5000 -2.5000\n"), out.str());
        }

        TEST_METHOD(WriteErrorFormat) {
            std::ostringstream out;
            writeError(out, 4, "invalid number");
            Assert::AreEqual(std::string("4 ERROR: invalid number\n"), out.str());
        }
    };

    // 9. ProgramTests (4 тести)
    TEST_CLASS(ProgramTests) {
    public:
        TEST_METHOD(ProcessAllMatchesChapter1) {
            std::istringstream in(
                "REFLECT\n4 3 -1 4 2\n\n"
                "SCALE 2.5\n4 3 -1 4 2\n\n"
                "ADD\n4 3 -1 4 2\n4 1 5 -2 0.5\n\n"
                "ADD\n3 1 2 3\n2 4 5\n\n"
                "SCALE 2,5\n3 1 2 3\n\n"
                "REFLECT\n3 1 2\n"
            );
            std::ostringstream fileOut, screenOut;
            int count = processAll<double>(in, fileOut, screenOut);
            Assert::AreEqual(6, count);
            Assert::AreEqual(fileOut.str(), screenOut.str());
            std::string expected =
                "1 REFLECT = 2.0000 4.0000 -1.0000 3.0000\n"
                "2 SCALE 2.5 = 7.5000 -2.5000 10.0000 5.0000\n"
                "3 ADD = 4.0000 4.0000 2.0000 2.5000\n"
                "4 ERROR: vector sizes differ: 3 and 2\n"
                "5 ERROR: invalid constant\n"
                "6 ERROR: wrong number of elements\n";
            Assert::AreEqual(expected, fileOut.str());
        }

        TEST_METHOD(ProcessAllBoundaryValues) {
            std::istringstream in(
                "REFLECT\n1 7\n\n"
                "SCALE 0\n3 5 -5 0.5\n\n"
                "SCALE -1\n3 0 2 -2\n"
            );
            std::ostringstream fileOut, screenOut;
            int count = processAll<double>(in, fileOut, screenOut);
            Assert::AreEqual(3, count);
            std::string expected =
                "1 REFLECT = 7.0000\n"
                "2 SCALE 0 = 0.0000 0.0000 0.0000\n"
                "3 SCALE -1 = 0.0000 -2.0000 2.0000\n";
            Assert::AreEqual(expected, fileOut.str());
        }

        TEST_METHOD(ErrorDoesNotAffectFollowingBlocks) {
            std::istringstream in(
                "MUL\n2 1 2\n\n"
                "ADD\n2 1.5 2.5\n2 -1.5 0.5\n"
            );
            std::ostringstream fileOut, screenOut;
            int count = processAll<double>(in, fileOut, screenOut);
            Assert::AreEqual(2, count);
            std::string expected =
                "1 ERROR: unknown operation\n"
                "2 ADD = 0.0000 3.0000\n";
            Assert::AreEqual(expected, fileOut.str());
        }

        TEST_METHOD(StopsOnEndLine) {
            std::istringstream in(
                "REFLECT\n2 1 2\n\n"
                "END\n\n"
                "REFLECT\n2 9 8\n"
            );
            std::ostringstream fileOut, screenOut;
            int count = processAll<double>(in, fileOut, screenOut);
            Assert::AreEqual(1, count);
            Assert::AreEqual(std::string("1 REFLECT = 2.0000 1.0000\n"), fileOut.str());
        }
    };
}
