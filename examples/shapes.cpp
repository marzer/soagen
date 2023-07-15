// This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "shapes.hpp"

using namespace soagen::examples;

int main()
{
	spheres p;
	spheres p2;

	p.reserve(10);

	p = std::move(p2);
	p.swap(p2);
	p2 = p;

	p.pop_back();
	p.resize(10);

	p2.push_back(1.0f, 2.0f, 3.0f);
	p2.insert(0, 4.0f, 5.0f, 6.0f);
	p2.emplace_back(7.0f, 8.0f, 9.0f);
	p2.emplace(0, 10.0f, 11.0f, 12.0f);

	[[maybe_unused]] auto back = p2.back();

	return 0;
}
