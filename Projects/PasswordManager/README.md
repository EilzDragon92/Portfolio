**Vault Format:** 
```
Magic Number (4 bytes) | Salt (16 bytes) | IV (12 bytes) | Encrypted Data | Tag (16 bytes)
```

**Encrypted Data Format:** 
```
Entry Count (4 bytes) | Entries
```

**Entry Example:** 
```
{ 
	site: "Google", 
	acc: "username@google.com", 
	pw: "password" 
}
```
