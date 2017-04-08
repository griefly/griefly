package main

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestParseVersion(t *testing.T) {
	testCases := []struct {
		base     string
		checking string
		less     bool
	}{
		{"v0.1", "v0.2", true},
		{"v0.2", "v0.1", false},
		{"v0.1", "v0.1.1", true},
		{"v0.1", "v0.1.0", true},
		{"v0.1.0.42", "v0.1.0.56", true},
		{"v0.1.0-42", "v0.1.0-56", true},
		{"v0.1.0", "v0.1.0-56", true},
		{"v0.4.0-97-ge220", "v0.4.0-107-g5590", true},
	}

	for _, cs := range testCases {
		assert.Equal(t, cs.less, parseVersion(cs.base).less(parseVersion(cs.checking)),
			"base: %s, checking: %s", cs.base, cs.checking)
	}
}
