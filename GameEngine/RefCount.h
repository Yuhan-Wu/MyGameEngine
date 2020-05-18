#pragma once

struct RefCount {
	int sp_count;
	int wp_count;

	RefCount(int sp_count, int wp_count) : sp_count{ sp_count }, wp_count{ wp_count } {
	}
};
