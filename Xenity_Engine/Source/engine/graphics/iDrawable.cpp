// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "iDrawable.h"

#include "graphics.h"

void IDrawable::RemoveReferences()
{
	Graphics::RemoveDrawable(this);
}

void IDrawable::SetOrderInLayer(int orderInLayer)
{
	m_orderInLayer = orderInLayer;
	Graphics::s_needUpdateUIOrdering = true;
}