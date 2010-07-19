/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
#include "CppRewriter.h"
#include <TypeVisitor.h>
#include <NameVisitor.h>
#include <CoreTypes.h>
#include <Symbols.h>
#include <Literals.h>
#include <Names.h>
#include <Scope.h>
#include <cplusplus/Overview.h>

#include <QtCore/QVarLengthArray>
#include <QtCore/QDebug>

using namespace CPlusPlus;

class CPlusPlus::Rewrite
{
public:
    Rewrite(Control *control, SubstitutionEnvironment *env)
        : control(control), env(env), rewriteType(this), rewriteName(this) {}

    class RewriteType: public TypeVisitor
    {
        Rewrite *rewrite;
        QList<FullySpecifiedType> temps;

        Control *control() const
        { return rewrite->control; }

        void accept(const FullySpecifiedType &ty)
        {
            TypeVisitor::accept(ty.type());
            unsigned flags = ty.flags();
            flags |= temps.back().flags();
            temps.back().setFlags(flags);
        }

    public:
        RewriteType(Rewrite *r): rewrite(r) {}

        FullySpecifiedType operator()(const FullySpecifiedType &ty)
        {
            accept(ty);
            return temps.takeLast();
        }

        virtual void visit(UndefinedType *)
        {
            temps.append(FullySpecifiedType());
        }

        virtual void visit(VoidType *)
        {
            temps.append(control()->voidType());
        }

        virtual void visit(IntegerType *type)
        {
            temps.append(control()->integerType(type->kind()));
        }

        virtual void visit(FloatType *type)
        {
            temps.append(control()->floatType(type->kind()));
        }

        virtual void visit(PointerToMemberType *type)
        {
            const Name *memberName = rewrite->rewriteName(type->memberName());
            const FullySpecifiedType elementType = rewrite->rewriteType(type->elementType());
            temps.append(control()->pointerToMemberType(memberName, elementType));
        }

        virtual void visit(PointerType *type)
        {
            const FullySpecifiedType elementType = rewrite->rewriteType(type->elementType());
            temps.append(control()->pointerType(elementType));
        }

        virtual void visit(ReferenceType *type)
        {
            const FullySpecifiedType elementType = rewrite->rewriteType(type->elementType());
            temps.append(control()->referenceType(elementType));
        }

        virtual void visit(ArrayType *type)
        {
            const FullySpecifiedType elementType = rewrite->rewriteType(type->elementType());
            temps.append(control()->arrayType(elementType, type->size()));
        }

        virtual void visit(NamedType *type)
        {
            FullySpecifiedType ty = rewrite->env->apply(type->name(), rewrite);
            if (! ty->isUndefinedType())
                temps.append(ty);
            else {
                const Name *name = rewrite->rewriteName(type->name());
                temps.append(control()->namedType(name));
            }
        }

        virtual void visit(Function *type)
        {
            Function *funTy = control()->newFunction(0, 0);
            funTy->copy(type);

            funTy->setName(rewrite->rewriteName(type->name()));

            funTy->setReturnType(rewrite->rewriteType(type->returnType()));

            for (unsigned i = 0; i < type->argumentCount(); ++i) {
                Symbol *arg = type->argumentAt(i);

                Argument *newArg = control()->newArgument(0, 0);
                newArg->copy(arg);
                newArg->setName(rewrite->rewriteName(arg->name()));
                newArg->setType(rewrite->rewriteType(arg->type()));

                funTy->arguments()->enterSymbol(newArg);
            }

            temps.append(funTy);
        }

        virtual void visit(Namespace *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(Class *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(Enum *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ForwardClassDeclaration *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ObjCClass *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ObjCProtocol *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ObjCMethod *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ObjCForwardClassDeclaration *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

        virtual void visit(ObjCForwardProtocolDeclaration *type)
        {
            qWarning() << Q_FUNC_INFO;
            temps.append(type);
        }

    };

    class RewriteName: public NameVisitor
    {
        Rewrite *rewrite;
        QList<const Name *> temps;

        Control *control() const
        { return rewrite->control; }

        const Identifier *identifier(const Identifier *other) const
        {
            if (! other)
                return 0;

            return control()->findOrInsertIdentifier(other->chars(), other->size());
        }

    public:
        RewriteName(Rewrite *r): rewrite(r) {}

        const Name *operator()(const Name *name)
        {
            if (! name)
                return 0;

            accept(name);
            return temps.takeLast();
        }

        virtual void visit(const QualifiedNameId *name)
        {
            const Name *base = rewrite->rewriteName(name->base());
            const Name *n = rewrite->rewriteName(name->name());
            temps.append(control()->qualifiedNameId(base, n));
        }

        virtual void visit(const NameId *name)
        {
            temps.append(control()->nameId(identifier(name->identifier())));
        }

        virtual void visit(const TemplateNameId *name)
        {
            QVarLengthArray<FullySpecifiedType, 8> args(name->templateArgumentCount());
            for (unsigned i = 0; i < name->templateArgumentCount(); ++i)
                args[i] = rewrite->rewriteType(name->templateArgumentAt(i));
            temps.append(control()->templateNameId(identifier(name->identifier()), args.data(), args.size()));
        }

        virtual void visit(const DestructorNameId *name)
        {
            temps.append(control()->destructorNameId(identifier(name->identifier())));
        }

        virtual void visit(const OperatorNameId *name)
        {
            temps.append(control()->operatorNameId(name->kind()));
        }

        virtual void visit(const ConversionNameId *name)
        {
            FullySpecifiedType ty = rewrite->rewriteType(name->type());
            temps.append(control()->conversionNameId(ty));
        }

        virtual void visit(const SelectorNameId *name)
        {
            QVarLengthArray<const Name *, 8> names(name->nameCount());
            for (unsigned i = 0; i < name->nameCount(); ++i)
                names[i] = rewrite->rewriteName(name->nameAt(i));
            temps.append(control()->selectorNameId(names.constData(), names.size(), name->hasArguments()));
        }
    };

public: // attributes
    Control *control;
    SubstitutionEnvironment *env;
    RewriteType rewriteType;
    RewriteName rewriteName;
};

SubstitutionEnvironment::SubstitutionEnvironment()
    : _scope(0)
{
}

FullySpecifiedType SubstitutionEnvironment::apply(const Name *name, Rewrite *rewrite) const
{
    if (name) {
        for (int index = _substs.size() - 1; index != -1; --index) {
            const Substitution *subst = _substs.at(index);

            FullySpecifiedType ty = subst->apply(name, rewrite);
            if (! ty->isUndefinedType())
                return ty;
        }
    }

    return FullySpecifiedType();
}

void SubstitutionEnvironment::enter(Substitution *subst)
{
    _substs.append(subst);
}

void SubstitutionEnvironment::leave()
{
    _substs.removeLast();
}

Scope *SubstitutionEnvironment::scope() const
{
    return _scope;
}

Scope *SubstitutionEnvironment::switchScope(Scope *scope)
{
    Scope *previous = _scope;
    _scope = scope;
    return previous;
}

const LookupContext &SubstitutionEnvironment::context() const
{
    return _context;
}

void SubstitutionEnvironment::setContext(const LookupContext &context)
{
    _context = context;
}

SubstitutionMap::SubstitutionMap()
{

}

SubstitutionMap::~SubstitutionMap()
{

}

void SubstitutionMap::bind(const Name *name, const FullySpecifiedType &ty)
{
    _map.append(qMakePair(name, ty));
}

FullySpecifiedType SubstitutionMap::apply(const Name *name, Rewrite *) const
{
    for (int n = _map.size() - 1; n != -1; --n) {
        const QPair<const Name *, FullySpecifiedType> &p = _map.at(n);

        if (name->isEqualTo(p.first))
            return p.second;
    }

    return FullySpecifiedType();
}


UseQualifiedNames::UseQualifiedNames()
{

}

UseQualifiedNames::~UseQualifiedNames()
{

}

FullySpecifiedType UseQualifiedNames::apply(const Name *name, Rewrite *rewrite) const
{
    SubstitutionEnvironment *env = rewrite->env;
    Scope *scope = env->scope();

    if (! scope)
        return FullySpecifiedType();

    const LookupContext &context = env->context();
    Control *control = rewrite->control;

    const QList<LookupItem> results = context.lookup(name, scope);
    foreach (const LookupItem &r, results) {
        if (Symbol *d = r.declaration()) {
            const Name *n = 0;
            foreach (const Name *c,  LookupContext::fullyQualifiedName(d)) {
                if (! n)
                    n = c;
                else
                    n = control->qualifiedNameId(n, c);
            }

            return control->namedType(n);
        }

        return r.type();
    }

    return FullySpecifiedType();
}


FullySpecifiedType CPlusPlus::rewriteType(const FullySpecifiedType &type,
                                          SubstitutionEnvironment *env,
                                          Control *control)
{
    Rewrite rewrite(control, env);
    return rewrite.rewriteType(type);
}

const Name *CPlusPlus::rewriteName(const Name *name,
                                   SubstitutionEnvironment *env,
                                   Control *control)
{
    Rewrite rewrite(control, env);
    return rewrite.rewriteName(name);
}
